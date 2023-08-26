#ifndef _PIN_CONFIG_H_
#define _PIN_CONFIG_H_

/* ------------------------------------------------------------------------- */
/*                DEBUG (uncomment to open the Debug function)               */
/* ------------------------------------------------------------------------- */

//// Sử dụng Serial
// #define ENABLE_DEBUG

#if defined(ENABLE_DEBUG)
#define Debug Serial
#define DEBUG_BEGIN(...) Debug.begin(__VA_ARGS__)
#define DEBUG_WRITE(...) Debug.write(__VA_ARGS__)
#define DEBUG_PRINT(...) Debug.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Debug.println(__VA_ARGS__)
#else
#define DEBUG_BEGIN(...)
#define DEBUG_WRITE(...)
#define DEBUG_PRINT(...)
#define DEBUG_PRINTLN(...)
#endif

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
/*                          AHT20 (Nhiệt độ & Độ ẩm)                         */
/* ------------------------------------------------------------------------- */

//

/* ------------------------------------------------------------------------- */
/*                              BMP180 (Áp suất)                             */
/* ------------------------------------------------------------------------- */

//

/* ------------------------------------------------------------------------- */
/*                              ML8511 (Tia UV)                              */
/* ------------------------------------------------------------------------- */

//

/* ------------------------------------------------------------------------- */
/*                          PMS7003 (Bụi mịn PM2.5)                          */
/* ------------------------------------------------------------------------- */

//

/* ------------------------------------------------------------------------- */
/*                                    OLED                                   */
/* ------------------------------------------------------------------------- */

//

/* ------------------------------------------------------------------------- */

#endif