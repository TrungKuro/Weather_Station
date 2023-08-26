/**
 * Code test tất cả cảm biến với ESP32 WROOM-32, bao gồm:
 * - PMS7003, AHT20, BMP180
 *
 * Hiển thị các thông tin lên màn hình OLED 1.3
 *
 * Đồng thời gửi dữ liệu lên Firebase
 */

/* ------------------------------------------------------------------------- */
/*                                  LIBRARY                                  */
/* ------------------------------------------------------------------------- */

#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

/* ------------------------------------------------------------------------- */

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>

/* ------------------------------------------------------------------------- */

#include "Plantower_PMS7003.h"
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP085.h>

/* ------------------------------------------------------------------------- */
/*                                   DEFINE                                  */
/* ------------------------------------------------------------------------- */

#define OLED_SDA 21 // GPIO-21
#define OLED_SCL 22 // GPIO-22

/* ------------------------------------------------------------------------- */

// Tần suất cập nhập dữ liệu mới gửi lên Firebase, đơn vị (ms)
#define FREQ_UPDATE 10000

// Tần suất đọc dữ liệu mới gửi lên Serial, đơn vị (ms)
#define FREQ_READ 1500

// Tần suất đọc dữ liệu PMS7003, đơn vị (ms)
#define FREQ_READ_PMS7003 100

// Tần suất đọc dữ liệu AHT20, đơn vị (ms)
#define FREQ_READ_AHT20 500

// Tần suất đọc dữ liệu BMP180, đơn vị (ms)
#define FREQ_READ_BMP180 500

// Tần suất hiển thị nội dung mới lên màn Oled, đơn vị (ms)
#define FREQ_SHOW 3000

/* -------------------------------- FIREBASE ------------------------------- */

//// Insert your network credentials
#define WIFI_SSID "HshopLTK"          //!!!
#define WIFI_PASSWORD "HshopLTK@2311" //!!!

//// Insert Firebase project API Key
#define API_KEY "AIzaSyBzro27svhQQF076n2IN1asR_B2V6D92bw"

//// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://weatherstation-of-makerlab-default-rtdb.asia-southeast1.firebasedatabase.app/"

//// Để tạo đường dẫn trên Firebase
#define URL_TEMP "temp" // Nhiệt độ không khí (Temperature)
#define URL_HUMI "humi" // Độ ẩm không khí (Humidity)
//
#define URL_PRES "pres"        // Áp suất (Pressure)
#define URL_ALTI "alti"        // Độ cao (Altitude)
#define URL_PRES_SEA "presSea" // Áp suất (Pressure at Sealevel)
#define URL_ALTI_SEA "altiSea" // Độ cao (Real Altitude)
//
#define URL_UV "uv" // Cường độ UV (UV Intensity)
//
#define URL_PM1 "pm1"     // Bụi mịn PM1.0 (Particulate Matter)
#define URL_PM2_5 "pm2d5" // Bụi mịn PM2.5 (Particulate Matter)
#define URL_PM10 "pm10"   // Bụi mịn PM10 (Particulate Matter)

//// Tần suất giữa mỗi lần gửi data lên Firebase
#define FREQ_WRITE_FIREBASE 10000 // Đơn vị (ms), ~ 10 giây

/* ------------------------------------------------------------------------- */
/*                                   OBJECT                                  */
/* ------------------------------------------------------------------------- */

// Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

/* ------------------------------------------------------------------------- */

// Oled 1.3
Adafruit_SH1106 display(OLED_SDA, OLED_SCL);

/* ------------------------------------------------------------------------- */

// PMS7003
Plantower_PMS7003 pms7003 = Plantower_PMS7003();

// AHT20
Adafruit_AHTX0 aht;

// BMP180
Adafruit_BMP085 bmp;

/* ------------------------------------------------------------------------- */
/*                                  VARIABLE                                 */
/* ------------------------------------------------------------------------- */

// Lưu thời điểm cập nhập dữ liệu mới lên Firebase
unsigned long pushValue;

// Lưu thời điểm gửi dữ liệu mới lên Serial
unsigned long updateValue;

// Lưu thời điểm đọc các cảm biến
unsigned long readPMS7003;
unsigned long readAHT20;
unsigned long readBMP180;

// Lưu thời điểm hiển thị nội dung mới lên Oled
unsigned long showValue;

/* ------------------------------------------------------------------------- */

// Lưu các giá trị của PMS7003
int pm1;   // PM1.0
int pm2d5; // PM2.5
int pm10;  // PM10

// Lưu các giá trị của AHT20
sensors_event_t temp, humi;

// Lưu các giá trị của BMP180
int pres;      // Áp suất (Pressure)
float alti;    // Độ cao (Altitude)
int presSea;   // Áp suất dựa theo mực nước biển
float altiSea; // Độ cao so với mực nước biển

/* ------------------------------------------------------------------------- */

enum WeatherContent
{
  DUST,
  TEMP,
  HUMI,
  PRES,
  ALTI,
};
WeatherContent menu = DUST;

/* ------------------------------------------------------------------------- */

bool signupOK = false;

/* ------------------------------------------------------------------------- */
/*                                RUN ONE TIME                               */
/* ------------------------------------------------------------------------- */

void setup()
{
  // Mở cổng UART0 - Debug
  Serial.begin(115200);

  /* ----------------------------------------------------------------------- */

  // Khởi động và kết nối Wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* ----------------------------------------------------------------------- */

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", ""))
  {
    Serial.println("ok");
    signupOK = true;
  }
  else
  {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  /* ----------------------------------------------------------------------- */

  // Khởi động màn hình Oled 1.3inch (128x64)
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);

  // Since the buffer is intialized with an Adafruit splashscreen internally
  // This will display the splashscreen → Clear the buffer
  display.clearDisplay();

  // Cài đặt văn bản hiển thị
  display.setTextColor(WHITE);

  /* ----------------------------------------------------------------------- */

  // Mở cổng UART2 - Đọc cảm biến PMS7003
  Serial2.begin(9600);
  pms7003.init(&Serial2);

  // Khởi động cảm biến AHT20
  if (!aht.begin())
  {
    Serial.println("Could not find AHT? Check wiring");
    while (1)
      delay(10);
  }
  Serial.println("AHT10 or AHT20 found");

  // Khởi động cảm biến BMP180
  if (!bmp.begin())
  {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1)
    {
    }
  }
}

/* ------------------------------------------------------------------------- */
/*                                    MAIN                                   */
/* ------------------------------------------------------------------------- */

void loop()
{
  read_PMS7003(); // Đọc PMS7003
  read_AHT20();   // Đọc AHT20
  read_BMP180();  // Đọc BMP180

  push_Firebase(); // Đẩy lên Firebase

  // Mỗi "FREQ_SHOW" giây hiển thị nội dung mới trên Oled
  if (millis() - showValue >= FREQ_SHOW)
  {
    showValue = millis();

    display_Oled();
  }

  // Mỗi "FREQ_READ" giây hiển thị toàn bộ dữ liệu một lần
  if (millis() - updateValue >= FREQ_READ)
  {
    updateValue = millis();

    // In dữ liệu của PMS7003
    Serial.print("PM1.0 (ug/m3): ");
    Serial.println(pm1);
    Serial.print("PM2.5(ug/m3) : ");
    Serial.println(pm2d5);
    Serial.print("PM10 (ug/m3) : ");
    Serial.println(pm10);

    // In dữ liệu của AHT20
    Serial.print("Temperature (ºC): ");
    Serial.println(temp.temperature);
    Serial.print("Humidity (%RH)  : ");
    Serial.println(humi.relative_humidity);

    // In dữ liệu của BMP180
    Serial.print("Pressure (Pa): ");
    Serial.println(pres);
    Serial.print("Altitude (m) : ");
    Serial.println(alti);
    Serial.print("Pressure Sea (Pa): ");
    Serial.println(presSea);
    Serial.print("Altitude Sea (m) : ");
    Serial.println(altiSea);

    // End
    Serial.println("----- ----- ----- ----- -----");
  }
}

/* ------------------------------------------------------------------------- */
/*                                  FUNCTION                                 */
/* ------------------------------------------------------------------------- */

// Hàm xử lý dữ liệu từ PMS7003
void read_PMS7003()
{
  if (millis() - readPMS7003 >= FREQ_READ_PMS7003)
  {
    readPMS7003 = millis();
    //
    pms7003.updateFrame();
    //
    if (pms7003.hasNewData())
    {
      pm1 = pms7003.getPM_1_0();
      pm2d5 = pms7003.getPM_2_5();
      pm10 = pms7003.getPM_10_0();
    }
  }
}

// Hàm xử lý dữ liệu từ AHT20
void read_AHT20()
{
  if (millis() - readAHT20 >= FREQ_READ_AHT20)
  {
    readAHT20 = millis();
    //
    aht.getEvent(&humi, &temp);
  }
}

// Hàm xử lý dữ liệu từ BMP180
void read_BMP180()
{
  if (millis() - readBMP180 >= FREQ_READ_BMP180)
  {
    readBMP180 = millis();
    //
    pres = bmp.readPressure();
    /**
     * Tính toán độ cao (altitude)
     * Giả sử áp suất khí quyển 'tiêu chuẩn'
     * Là 1013,25 millibar = 101325 Pascal
     */
    alti = bmp.readAltitude();
    //
    presSea = bmp.readSealevelPressure();
    /**
     * Bạn có thể đo độ cao chính xác hơn
     * Nếu bạn biết áp suất mực nước biển hiện tại
     * Sẽ thay đổi theo thời tiết
     *
     * Nếu là 1015 millibar thì bằng 101500 Pascals
     */
    altiSea = bmp.readAltitude(101500);
  }
}

/* ------------------------------------------------------------------------- */

// Hàm xử lý nội dung hiển thị lên OLED
void display_Oled()
{
  display.clearDisplay();  // Xóa nội dung cũ
  display.setCursor(0, 0); // Cài đặt con trỏ

  /* ----------------------------------------------------------------------- */

  switch (menu)
  {
  case DUST:
  {
    display.setTextSize(1);
    display.print("Dust (ug/m3).");
    display.print(F("\n\n"));
    //
    display.print("PM 1.0: ");
    display.print(pm1);
    display.print(F("\n\n"));
    //
    display.print("PM 2.5: ");
    display.print(pm2d5);
    display.print(F("\n\n"));
    //
    display.print("PM 10 : ");
    display.print(pm10);
    display.print(F("\n\n"));
  }
    menu = TEMP;
    break;
  case TEMP:
  {
    display.setTextSize(1);
    display.print("Temperature.");
    display.print(F("\n\n"));
    //
    display.setTextSize(2);
    display.print(temp.temperature);
    display.println(F(" \xF8\x43"));
  }
    menu = HUMI;
    break;
  case HUMI:
  {
    display.setTextSize(1);
    display.print("Humidity.");
    display.print(F("\n\n"));
    //
    display.setTextSize(2);
    display.print(humi.relative_humidity);
    display.println(" %RH");
  }
    menu = PRES;
    break;
  case PRES:
  {
    display.setTextSize(1);
    display.print("Pressure (Pa).");
    display.print(F("\n\n"));
    //
    display.setTextSize(1);
    display.println(">> Land: ");
    display.setTextSize(2);
    display.println(pres);
    //
    display.setTextSize(1);
    display.println(">> Sea : ");
    display.setTextSize(2);
    display.println(presSea);
  }
    menu = ALTI;
    break;
  case ALTI:
  {
    display.setTextSize(1);
    display.print("Altitude (meter).");
    display.print(F("\n\n"));
    //
    display.setTextSize(1);
    display.println(">> Land: ");
    display.setTextSize(2);
    display.println(alti);
    //
    display.setTextSize(1);
    display.println(">> Sea : ");
    display.setTextSize(2);
    display.println(altiSea);
  }
    menu = DUST;
    break;
  }

  /* ----------------------------------------------------------------------- */

  display.display(); // Cho in nội dung mới
}

/* ------------------------------------------------------------------------- */

// Hàm xử lý đẩy dữ liệu lên Firebase
void push_Firebase()
{
  if (Firebase.ready() && signupOK && (millis() - pushValue > FREQ_UPDATE || pushValue == 0))
  {
    pushValue = millis();

    /* ------------------------------ PMS7003 ------------------------------ */

    // PM1.0
    if (Firebase.RTDB.setInt(&fbdo, URL_PM1, pm1))
    {
      Serial.println("PASSED");
      Serial.println("Path: " + fbdo.dataPath());
      Serial.println("Type: " + fbdo.dataType());
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("Reason: " + fbdo.errorReason());
    }

    // PM2.5
    if (Firebase.RTDB.setInt(&fbdo, URL_PM2_5, pm2d5))
    {
      Serial.println("PASSED");
      Serial.println("Path: " + fbdo.dataPath());
      Serial.println("Type: " + fbdo.dataType());
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("Reason: " + fbdo.errorReason());
    }

    // PM10
    if (Firebase.RTDB.setInt(&fbdo, URL_PM10, pm10))
    {
      Serial.println("PASSED");
      Serial.println("Path: " + fbdo.dataPath());
      Serial.println("Type: " + fbdo.dataType());
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("Reason: " + fbdo.errorReason());
    }

    /* ------------------------------- AHT20 ------------------------------- */

    // Nhiệt độ
    if (Firebase.RTDB.setFloat(&fbdo, URL_TEMP, temp.temperature))
    {
      Serial.println("PASSED");
      Serial.println("Path: " + fbdo.dataPath());
      Serial.println("Type: " + fbdo.dataType());
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("Reason: " + fbdo.errorReason());
    }

    // Độ ẩm
    if (Firebase.RTDB.setFloat(&fbdo, URL_HUMI, humi.relative_humidity))
    {
      Serial.println("PASSED");
      Serial.println("Path: " + fbdo.dataPath());
      Serial.println("Type: " + fbdo.dataType());
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("Reason: " + fbdo.errorReason());
    }

    /* ------------------------------- BMP180 ------------------------------ */

    // Áp suất
    if (Firebase.RTDB.setInt(&fbdo, URL_PRES, pres))
    {
      Serial.println("PASSED");
      Serial.println("Path: " + fbdo.dataPath());
      Serial.println("Type: " + fbdo.dataType());
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("Reason: " + fbdo.errorReason());
    }

    // Độ cao
    if (Firebase.RTDB.setInt(&fbdo, URL_ALTI, alti))
    {
      Serial.println("PASSED");
      Serial.println("Path: " + fbdo.dataPath());
      Serial.println("Type: " + fbdo.dataType());
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("Reason: " + fbdo.errorReason());
    }

    // Áp suất (mực nước biển)
    if (Firebase.RTDB.setInt(&fbdo, URL_PRES_SEA, presSea))
    {
      Serial.println("PASSED");
      Serial.println("Path: " + fbdo.dataPath());
      Serial.println("Type: " + fbdo.dataType());
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("Reason: " + fbdo.errorReason());
    }

    // Độ cao (mực nước biển)
    if (Firebase.RTDB.setInt(&fbdo, URL_ALTI_SEA, altiSea))
    {
      Serial.println("PASSED");
      Serial.println("Path: " + fbdo.dataPath());
      Serial.println("Type: " + fbdo.dataType());
    }
    else
    {
      Serial.println("FAILED");
      Serial.println("Reason: " + fbdo.errorReason());
    }
  }
}