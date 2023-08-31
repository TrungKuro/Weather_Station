#ifndef _PIN_CONFIG_H_
#define _PIN_CONFIG_H_

/* ------------------------------------------------------------------------- */
/*                DEBUG (uncomment to open the Debug function)               */
/* ------------------------------------------------------------------------- */

//// Sử dụng Serial
#define ENABLE_DEBUG

#if defined(ENABLE_DEBUG)
#define Debug Serial
#define DEBUG_BEGIN(...) Debug.begin(__VA_ARGS__)
#define DEBUG_WRITE(...) Debug.write(__VA_ARGS__)
#define DEBUG_PRINT(...) Debug.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Debug.println(__VA_ARGS__)
#define DEBUG_PRINTF(...) Debug.printf(__VA_ARGS__)
#else
#define DEBUG_BEGIN(...)
#define DEBUG_WRITE(...)
#define DEBUG_PRINT(...)
#define DEBUG_PRINTLN(...)
#define DEBUG_PRINTF(...)
#endif

/* ------------------------------------------------------------------------- */

// Khoảng tạm nghỉ trước khi bắt đầu chạy, đơn vị (ms)
#define WAIT 1000

// Tần suất đọc dữ liệu mới gửi lên Serial, đơn vị (ms)
#define FREQ_SHOW_SERIAL 1000

/* ------------------------------------------------------------------------- */
/*                                  FIREBASE                                 */
/* ------------------------------------------------------------------------- */

//// Insert your network credentials
#define WIFI_SSID "HshopLTK"          //!!!
#define WIFI_PASSWORD "HshopLTK@2311" //!!!

//// Insert Firebase project API Key
#define API_KEY "AIzaSyBzro27svhQQF076n2IN1asR_B2V6D92bw"

//// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://weatherstation-of-makerlab-default-rtdb.asia-southeast1.firebasedatabase.app/"

//// Để tạo đường dẫn trên Firebase
#define URL_PM1 "pm1"     // Bụi mịn PM1.0 (Particulate Matter)
#define URL_PM2_5 "pm2d5" // Bụi mịn PM2.5 (Particulate Matter)
#define URL_PM10 "pm10"   // Bụi mịn PM10 (Particulate Matter)
//
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
#define URL_TIME_STAMP "time" // Timestamp

/* ------------------------------------------------------------------------- */

//// Tần suất giữa mỗi lần gửi data lên Firebase
#define FREQ_WRITE_FIREBASE 60000 // Đơn vị (ms), ~ 1 phút

//// Thứ tự địa chỉ danh mục trong mảng của từng giá trị cảm biến
#define VALUE_PM1 0
#define VALUE_PM2_5 1
#define VALUE_PM10 2
#define VALUE_TEMP 3
#define VALUE_HUMI 4
#define VALUE_PRES 5
#define VALUE_ALTI 6
#define VALUE_PRES_SEA 7
#define VALUE_ALTI_SEA 8
#define VALUE_UV 9
#define TOTAL_VALUE 10

/* ------------------------------------------------------------------------- */
/*                          PMS7003 (Bụi mịn PM2.5)                          */
/* ------------------------------------------------------------------------- */

// Tần suất đọc dữ liệu PMS7003, đơn vị (ms)
#define FREQ_READ_PMS7003 500

/* ------------------------------------------------------------------------- */
/*                          AHT20 (Nhiệt độ & Độ ẩm)                         */
/* ------------------------------------------------------------------------- */

// Tần suất đọc dữ liệu AHT20, đơn vị (ms)
#define FREQ_READ_AHT20 500

/* ------------------------------------------------------------------------- */
/*                              BMP180 (Áp suất)                             */
/* ------------------------------------------------------------------------- */

// Tần suất đọc dữ liệu BMP180, đơn vị (ms)
#define FREQ_READ_BMP180 500

/* ------------------------------------------------------------------------- */
/*                              ML8511 (Tia UV)                              */
/* ------------------------------------------------------------------------- */

// Tần suất đọc dữ liệu ML8511, đơn vị (ms)
#define FREQ_READ_ML8511 500

/* ------------------------------------------------------------------------- */
/*                                    OLED                                   */
/* ------------------------------------------------------------------------- */

#define OLED_SDA 21 // GPIO-21
#define OLED_SCL 22 // GPIO-22

// Tần suất hiển thị nội dung mới lên màn Oled, đơn vị (ms)
#define FREQ_SHOW_OLED 3000

/* ------------------------------------------------------------------------- */

#endif