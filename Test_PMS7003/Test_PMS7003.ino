/**
 * Code test cảm biến PMS7003 với ESP32 WROOM-32
 * Giao thức UART2, trong đó RX2 là GPIO16 và TX2 là GPIO17
 *
 * Lưu ý, Baudrate mặc định của cảm biến là 9600
 * Khác với Baudrate giữa máy tính và ESP32
 */

/* ------------------------------------------------------------------------- */
/*                                  LIBRARY                                  */
/* ------------------------------------------------------------------------- */

#include "Plantower_PMS7003.h"

/* ------------------------------------------------------------------------- */
/*                                   OBJECT                                  */
/* ------------------------------------------------------------------------- */

Plantower_PMS7003 pms7003 = Plantower_PMS7003();

/* ------------------------------------------------------------------------- */
/*                                  VARIABLE                                 */
/* ------------------------------------------------------------------------- */

char output[256];

/* ------------------------------------------------------------------------- */
/*                                RUN ONE TIME                               */
/* ------------------------------------------------------------------------- */

void setup()
{
  Serial.begin(115200);

  Serial2.begin(9600);
  pms7003.init(&Serial2);
  // pms7003.debug = true;
}

/* ------------------------------------------------------------------------- */
/*                                    MAIN                                   */
/* ------------------------------------------------------------------------- */

void loop()
{
  pms7003.updateFrame();

  if (pms7003.hasNewData())
  {

    sprintf(output, "\nSensor Version: %d    Error Code: %d\n",
            pms7003.getHWVersion(),
            pms7003.getErrorCode());
    Serial.print(output);

    sprintf(output, "    PM1.0 (ug/m3): %2d     [atmos: %d]\n",
            pms7003.getPM_1_0(),
            pms7003.getPM_1_0_atmos());
    Serial.print(output);
    sprintf(output, "    PM2.5 (ug/m3): %2d     [atmos: %d]\n",
            pms7003.getPM_2_5(),
            pms7003.getPM_2_5_atmos());
    Serial.print(output);
    sprintf(output, "    PM10  (ug/m3): %2d     [atmos: %d]\n",
            pms7003.getPM_10_0(),
            pms7003.getPM_10_0_atmos());
    Serial.print(output);

    sprintf(output, "\n    RAW: %2d[>0.3] %2d[>0.5] %2d[>1.0] %2d[>2.5] %2d[>5.0] %2d[>10]\n",
            pms7003.getRawGreaterThan_0_3(),
            pms7003.getRawGreaterThan_0_5(),
            pms7003.getRawGreaterThan_1_0(),
            pms7003.getRawGreaterThan_2_5(),
            pms7003.getRawGreaterThan_5_0(),
            pms7003.getRawGreaterThan_10_0());
    Serial.print(output);
  }
}
