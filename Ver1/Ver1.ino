/**
 * Code test tất cả cảm biến với ESP32 WROOM-32, bao gồm:
 * - PMS7003, AHT20, BMP180, ML8511
 *
 * Hiển thị các thông tin lên màn hình OLED 1.3
 *
 * Đồng thời gửi dữ liệu lên Firebase
 */

/* ------------------------------------------------------------------------- */
/*                                  LIBRARY                                  */
/* ------------------------------------------------------------------------- */

#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
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
//... thư viện ML8511 (UV)

/* ------------------------------------------------------------------------- */

#include "Pin_Config.h"

/* ------------------------------------------------------------------------- */
/*                                   OBJECT                                  */
/* ------------------------------------------------------------------------- */

// Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

/* ------------------------------------------------------------------------- */

// NTP Client
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

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

// ML8511
//... đối tượng ML8511

/* ------------------------------------------------------------------------- */
/*                                  VARIABLE                                 */
/* ------------------------------------------------------------------------- */

// Lưu DATE và TIME từ NTP Client
String formattedDate = "";

// Lưu thời điểm cập nhập dữ liệu mới lên Firebase
// Sử dụng kết hợp với "FREQ_WRITE_FIREBASE"
unsigned long pushValue = 0;

// Lưu thời điểm gửi dữ liệu mới lên Serial
// Sử dụng kết hợp với "FREQ_SHOW_SERIAL"
unsigned long updateValue = 0;

// Lưu thời điểm đọc các cảm biến
unsigned long readPMS7003 = 0; // Sử dụng kết hợp với "FREQ_READ_PMS7003"
unsigned long readAHT20 = 0;   // Sử dụng kết hợp với "FREQ_READ_AHT20"
unsigned long readBMP180 = 0;  // Sử dụng kết hợp với "FREQ_READ_BMP180"
// unsigned long readML8511 = 0;  // Sử dụng kết hợp với "FREQ_READ_ML8511"

// Nhớ số lần lấy mẫu, để tính giá trị trung bình
int samplePMS7003 = 0;
int sampleAHT20 = 0;
int sampleBMP180 = 0;
// int sampleML8511 = 0;

// Lưu thời điểm hiển thị nội dung mới lên Oled
// Sử dụng kết hợp với "FREQ_SHOW_OLED"
unsigned long showValue = 0;

/* ------------------------------------------------------------------------- */

// Lưu các giá trị của PMS7003
int pm1 = 0;   // PM1.0
int pm2d5 = 0; // PM2.5
int pm10 = 0;  // PM10

// Lưu các giá trị của AHT20
sensors_event_t temp, humi;

// Lưu các giá trị của BMP180
int pres = 0;        // Áp suất (Pressure)
float alti = 0.0;    // Độ cao (Altitude)
int presSea = 0;     // Áp suất dựa theo mực nước biển
float altiSea = 0.0; // Độ cao so với mực nước biển

// Lưu các giá trị của ML8511
//...

// Lưu các giá trị trung bình lấy mẫu để gửi lên Firebase
float valueAverage[TOTAL_VALUE] = {0.0};

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
  DEBUG_BEGIN(115200);

  /* --------------------------------- Oled -------------------------------- */

  // Khởi động màn hình Oled 1.3inch (128x64)
  display.begin(SH1106_SWITCHCAPVCC, 0x3C);
  // Since the buffer is intialized with an Adafruit splashscreen internally
  // This will display the splashscreen → Clear the buffer
  display.clearDisplay();
  // Cài đặt văn bản hiển thị
  display.setTextColor(WHITE);

  DEBUG_PRINT(F("Init Oled...\n\n"));
  //
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print(F("Init Oled..."));
  display.display();

  /* --------------------------------- Wifi -------------------------------- */

  // Khởi động và kết nối Wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  DEBUG_PRINT(F("Connecting to Wi-Fi"));
  while (WiFi.status() != WL_CONNECTED)
  {
    DEBUG_PRINT(F("."));
    delay(300);
  }
  DEBUG_PRINTLN();
  DEBUG_PRINT(F("Connected with IP: "));
  DEBUG_PRINTLN(WiFi.localIP());

  DEBUG_PRINT(F("Init Wifi...\n\n"));
  //
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print(F("Init Wifi..."));
  display.display();

  /* ------------------------------ NTP Client ----------------------------- */

  // Khởi tạo NTPClient
  timeClient.begin();
  /**
   * Set offset time in seconds to adjust for your timezone
   * GMT -1 = -3600
   * GMT 0 = 0
   * GMT +1 = 3600
   *
   * Lệch một múi giờ, nghĩa là lệch 3600s
   * Múi giờ GMT tại Việt Nam là GMT+7 = 25200
   */
  timeClient.setTimeOffset(25200);

  DEBUG_PRINT(F("Init NTP Client...\n\n"));
  //
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print(F("Init NTP Client..."));
  display.display();

  /* ------------------------------- Firebase ------------------------------ */

  /* Assign the api key (required) */
  config.api_key = API_KEY;
  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;
  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", ""))
  {
    DEBUG_PRINTLN(F("ok"));
    signupOK = true;
  }
  else
  {
    DEBUG_PRINTF("%s\n", config.signer.signupError.message.c_str());
  }
  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h
  //
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  DEBUG_PRINT(F("Init Firebase...\n\n"));
  //
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print(F("Init Firebase..."));
  display.display();

  /* ------------------------------- PMS7003 ------------------------------- */

  // Mở cổng UART2 - Đọc cảm biến PMS7003
  Serial2.begin(9600);
  pms7003.init(&Serial2);

  DEBUG_PRINT(F("Init PMS7003...\n\n"));
  //
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print(F("Init PMS7003..."));
  display.display();

  /* -------------------------------- AHT20 -------------------------------- */

  // Khởi động cảm biến AHT20
  if (!aht.begin())
  {
    DEBUG_PRINTLN(F("Could not find AHT? Check wiring"));
    while (1)
      delay(10);
  }
  DEBUG_PRINTLN(F("AHT10 or AHT20 found"));

  DEBUG_PRINT(F("Init AHT20...\n\n"));
  //
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print(F("Init AHT20..."));
  display.display();

  /* -------------------------------- BMP180 ------------------------------- */

  // Khởi động cảm biến BMP180
  if (!bmp.begin())
  {
    DEBUG_PRINTLN(F("Could not find a valid BMP085 sensor, check wiring!"));
    while (1)
    {
    }
  }

  DEBUG_PRINT(F("Init BMP180...\n\n"));
  //
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print(F("Init BMP180..."));
  display.display();

  /* -------------------------------- ML8511 ------------------------------- */

  // Khởi động cảm biến ML8511
  //...

  /* ----------------------------------------------------------------------- */

  DEBUG_PRINT(F("All are ready...\n\n"));
  //
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.print(F("All are ready..."));
  display.display();
  //
  delay(WAIT);
}

/* ------------------------------------------------------------------------- */
/*                                    MAIN                                   */
/* ------------------------------------------------------------------------- */

void loop()
{
  read_PMS7003(); // Đọc PMS7003
  read_AHT20();   // Đọc AHT20
  read_BMP180();  // Đọc BMP180
  // read_ML8511(); // Đọc ML8511

  /* ----------------------------------------------------------------------- */

  // Hiển thị toàn bộ dữ liệu lên Serial (giá trị tức thời)
  display_Serial();

  // Hiển thị nội dung mới trên Oled (giá trị tức thời)
  display_Oled();

  /* ----------------------------------------------------------------------- */

  push_Firebase(); // Đẩy lên Firebase (giá trị trung bình mẫu)
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
    /* --------------------------------------------------------------------- */
    pms7003.updateFrame();
    //
    if (pms7003.hasNewData())
    {
      pm1 = pms7003.getPM_1_0();
      pm2d5 = pms7003.getPM_2_5();
      pm10 = pms7003.getPM_10_0();
    }
    /* --------------------------------------------------------------------- */
    if ((pm1 != 0) && (pm2d5 != 0) && (pm10 != 0))
    {
      valueAverage[VALUE_PM1] += pm1;
      valueAverage[VALUE_PM2_5] += pm2d5;
      valueAverage[VALUE_PM10] += pm10;
      //
      samplePMS7003++;
    }
  }
}

// Hàm xử lý dữ liệu từ AHT20
void read_AHT20()
{
  if (millis() - readAHT20 >= FREQ_READ_AHT20)
  {
    readAHT20 = millis();
    /* --------------------------------------------------------------------- */
    aht.getEvent(&humi, &temp);
    /* --------------------------------------------------------------------- */
    valueAverage[VALUE_TEMP] += temp.temperature;
    valueAverage[VALUE_HUMI] += humi.relative_humidity;
    //
    sampleAHT20++;
  }
}

// Hàm xử lý dữ liệu từ BMP180
void read_BMP180()
{
  if (millis() - readBMP180 >= FREQ_READ_BMP180)
  {
    readBMP180 = millis();
    /* --------------------------------------------------------------------- */
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
    /* --------------------------------------------------------------------- */
    valueAverage[VALUE_PRES] += pres;
    valueAverage[VALUE_ALTI] += alti;
    valueAverage[VALUE_PRES_SEA] += presSea;
    valueAverage[VALUE_ALTI_SEA] += altiSea;
    //
    sampleBMP180++;
  }
}

// Hàm xử lý dữ liệu từ ML8511
// void read_ML8511()
// {
//   //...
// }

/* ------------------------------------------------------------------------- */

// Hàm xử lý nội dung hiển thị lên OLED
void display_Oled()
{
  if (millis() - showValue >= FREQ_SHOW_OLED)
  {
    showValue = millis();
    //
    display.clearDisplay();  // Xóa nội dung cũ
    display.setCursor(0, 0); // Cài đặt con trỏ

    /* --------------------------------------------------------------------- */

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

    /* --------------------------------------------------------------------- */

    display.display(); // Cho in nội dung mới
  }
}

/* ------------------------------------------------------------------------- */

// Hàm xử lý nội dung hiển thị lên Serial
void display_Serial()
{
  if (millis() - updateValue >= FREQ_SHOW_SERIAL)
  {
    updateValue = millis();

    // In dữ liệu của PMS7003
    DEBUG_PRINT(F("PM1.0 (ug/m3): "));
    DEBUG_PRINT(valueAverage[VALUE_PM1]);
    DEBUG_PRINT(F(" | "));
    DEBUG_PRINTLN(samplePMS7003);
    DEBUG_PRINT(F("PM2.5(ug/m3) : "));
    DEBUG_PRINT(valueAverage[VALUE_PM2_5]);
    DEBUG_PRINT(F(" | "));
    DEBUG_PRINTLN(samplePMS7003);
    DEBUG_PRINT(F("PM10 (ug/m3) : "));
    DEBUG_PRINT(valueAverage[VALUE_PM10]);
    DEBUG_PRINT(F(" | "));
    DEBUG_PRINTLN(samplePMS7003);

    // In dữ liệu của AHT20
    DEBUG_PRINT(F("Temperature (ºC): "));
    DEBUG_PRINT(valueAverage[VALUE_TEMP]);
    DEBUG_PRINT(F(" | "));
    DEBUG_PRINTLN(sampleAHT20);
    DEBUG_PRINT(F("Humidity (%RH)  : "));
    DEBUG_PRINT(valueAverage[VALUE_HUMI]);
    DEBUG_PRINT(F(" | "));
    DEBUG_PRINTLN(sampleAHT20);

    // In dữ liệu của BMP180
    DEBUG_PRINT(F("Pressure (Pa): "));
    DEBUG_PRINT(valueAverage[VALUE_PRES]);
    DEBUG_PRINT(F(" | "));
    DEBUG_PRINTLN(sampleBMP180);
    DEBUG_PRINT(F("Altitude (m) : "));
    DEBUG_PRINT(valueAverage[VALUE_ALTI]);
    DEBUG_PRINT(F(" | "));
    DEBUG_PRINTLN(sampleBMP180);
    DEBUG_PRINT(F("Pressure Sea (Pa): "));
    DEBUG_PRINT(valueAverage[VALUE_PRES_SEA]);
    DEBUG_PRINT(F(" | "));
    DEBUG_PRINTLN(sampleBMP180);
    DEBUG_PRINT(F("Altitude Sea (m) : "));
    DEBUG_PRINT(valueAverage[VALUE_ALTI_SEA]);
    DEBUG_PRINT(F(" | "));
    DEBUG_PRINTLN(sampleBMP180);

    // End
    DEBUG_PRINTLN(F("----- ----- ----- ----- -----"));
  }
}

/* ------------------------------------------------------------------------- */

// Hàm xử lý đẩy dữ liệu lên Firebase
void push_Firebase()
{
  if (Firebase.ready() && signupOK && millis() - pushValue > FREQ_WRITE_FIREBASE)
  {
    pushValue = millis();

    /* ------------------------------ PMS7003 ------------------------------ */

    // PM1.0
    valueAverage[VALUE_PM1] /= samplePMS7003;
    if (Firebase.RTDB.setFloat(&fbdo, URL_PM1, valueAverage[VALUE_PM1]))
    {
      DEBUG_PRINTLN(F("PASSED"));
      DEBUG_PRINTLN("Path: " + fbdo.dataPath());
      DEBUG_PRINTLN("Type: " + fbdo.dataType());
    }
    else
    {
      DEBUG_PRINTLN(F("FAILED"));
      DEBUG_PRINTLN("Reason: " + fbdo.errorReason());
    }

    // PM2.5
    valueAverage[VALUE_PM2_5] /= samplePMS7003;
    if (Firebase.RTDB.setFloat(&fbdo, URL_PM2_5, valueAverage[VALUE_PM2_5]))
    {
      DEBUG_PRINTLN(F("PASSED"));
      DEBUG_PRINTLN("Path: " + fbdo.dataPath());
      DEBUG_PRINTLN("Type: " + fbdo.dataType());
    }
    else
    {
      DEBUG_PRINTLN(F("FAILED"));
      DEBUG_PRINTLN("Reason: " + fbdo.errorReason());
    }

    // PM10
    valueAverage[VALUE_PM10] /= samplePMS7003;
    if (Firebase.RTDB.setFloat(&fbdo, URL_PM10, valueAverage[VALUE_PM10]))
    {
      DEBUG_PRINTLN(F("PASSED"));
      DEBUG_PRINTLN("Path: " + fbdo.dataPath());
      DEBUG_PRINTLN("Type: " + fbdo.dataType());
    }
    else
    {
      DEBUG_PRINTLN(F("FAILED"));
      DEBUG_PRINTLN("Reason: " + fbdo.errorReason());
    }

    /* ------------------------------- AHT20 ------------------------------- */

    // Nhiệt độ
    valueAverage[VALUE_TEMP] /= sampleAHT20;
    if (Firebase.RTDB.setFloat(&fbdo, URL_TEMP, valueAverage[VALUE_TEMP]))
    {
      DEBUG_PRINTLN(F("PASSED"));
      DEBUG_PRINTLN("Path: " + fbdo.dataPath());
      DEBUG_PRINTLN("Type: " + fbdo.dataType());
    }
    else
    {
      DEBUG_PRINTLN(F("FAILED"));
      DEBUG_PRINTLN("Reason: " + fbdo.errorReason());
    }

    // Độ ẩm
    valueAverage[VALUE_HUMI] /= sampleAHT20;
    if (Firebase.RTDB.setFloat(&fbdo, URL_HUMI, valueAverage[VALUE_HUMI]))
    {
      DEBUG_PRINTLN(F("PASSED"));
      DEBUG_PRINTLN("Path: " + fbdo.dataPath());
      DEBUG_PRINTLN("Type: " + fbdo.dataType());
    }
    else
    {
      DEBUG_PRINTLN(F("FAILED"));
      DEBUG_PRINTLN("Reason: " + fbdo.errorReason());
    }

    /* ------------------------------- BMP180 ------------------------------ */

    // Áp suất
    valueAverage[VALUE_PRES] /= sampleBMP180;
    if (Firebase.RTDB.setFloat(&fbdo, URL_PRES, valueAverage[VALUE_PRES]))
    {
      DEBUG_PRINTLN(F("PASSED"));
      DEBUG_PRINTLN("Path: " + fbdo.dataPath());
      DEBUG_PRINTLN("Type: " + fbdo.dataType());
    }
    else
    {
      DEBUG_PRINTLN(F("FAILED"));
      DEBUG_PRINTLN("Reason: " + fbdo.errorReason());
    }

    // Độ cao
    valueAverage[VALUE_ALTI] /= sampleBMP180;
    if (Firebase.RTDB.setFloat(&fbdo, URL_ALTI, valueAverage[VALUE_ALTI]))
    {
      DEBUG_PRINTLN(F("PASSED"));
      DEBUG_PRINTLN("Path: " + fbdo.dataPath());
      DEBUG_PRINTLN("Type: " + fbdo.dataType());
    }
    else
    {
      DEBUG_PRINTLN(F("FAILED"));
      DEBUG_PRINTLN("Reason: " + fbdo.errorReason());
    }

    // Áp suất (mực nước biển)
    valueAverage[VALUE_PRES_SEA] /= sampleBMP180;
    if (Firebase.RTDB.setFloat(&fbdo, URL_PRES_SEA, valueAverage[VALUE_PRES_SEA]))
    {
      DEBUG_PRINTLN(F("PASSED"));
      DEBUG_PRINTLN("Path: " + fbdo.dataPath());
      DEBUG_PRINTLN("Type: " + fbdo.dataType());
    }
    else
    {
      DEBUG_PRINTLN(F("FAILED"));
      DEBUG_PRINTLN("Reason: " + fbdo.errorReason());
    }

    // Độ cao (mực nước biển)
    valueAverage[VALUE_ALTI_SEA] /= sampleBMP180;
    if (Firebase.RTDB.setFloat(&fbdo, URL_ALTI_SEA, valueAverage[VALUE_ALTI_SEA]))
    {
      DEBUG_PRINTLN(F("PASSED"));
      DEBUG_PRINTLN("Path: " + fbdo.dataPath());
      DEBUG_PRINTLN("Type: " + fbdo.dataType());
    }
    else
    {
      DEBUG_PRINTLN(F("FAILED"));
      DEBUG_PRINTLN("Reason: " + fbdo.errorReason());
    }

    /* ------------------------------- ML8511 ------------------------------ */

    // Cường độ tia UV
    // valueAverage[VALUE_UV] /= sampleML8511;
    // if (Firebase.RTDB.setFloat(&fbdo, URL_UV, valueAverage[VALUE_UV]))
    // {
    //   DEBUG_PRINTLN(F("PASSED"));
    //   DEBUG_PRINTLN("Path: " + fbdo.dataPath());
    //   DEBUG_PRINTLN("Type: " + fbdo.dataType());
    // }
    // else
    // {
    //   DEBUG_PRINTLN(F("FAILED"));
    //   DEBUG_PRINTLN("Reason: " + fbdo.errorReason());
    // }

    /* ----------------------------- NTP Client ---------------------------- */

    // Lấy dữ liệu thời gian hiện tại
    while (!timeClient.update())
    {
      timeClient.forceUpdate();
    }
    formattedDate = timeClient.getFormattedDate();
    //
    if (Firebase.RTDB.setString(&fbdo, URL_TIME_STAMP, formattedDate))
    {
      DEBUG_PRINTLN(F("PASSED"));
      DEBUG_PRINTLN("Path: " + fbdo.dataPath());
      DEBUG_PRINTLN("Type: " + fbdo.dataType());
    }
    else
    {
      DEBUG_PRINTLN(F("FAILED"));
      DEBUG_PRINTLN("Reason: " + fbdo.errorReason());
    }

    // End
    DEBUG_PRINTLN("----- ----- ----- ----- -----");

    /* --------------------------- Reset dữ liệu --------------------------- */

    for (int i = 0; i < TOTAL_VALUE; i++)
    {
      valueAverage[i] = 0.0;
    }
    //
    samplePMS7003 = 0;
    sampleAHT20 = 0;
    sampleBMP180 = 0;
    // sampleML8511 = 0;
  }
}