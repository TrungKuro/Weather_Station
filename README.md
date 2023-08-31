# Dự án trạm thời tiết

Đây là một `Trạm thiết bị`, có thể lắp đặt ngoài trời, với các thiết bị đo những chỉ số của môi trường. Trạm thiết bị này sẽ gửi dữ liệu theo chu kỳ tuần tự lên Cloud. Các dữ liệu được đưa lên, Cloud sẽ tổng hợp lại để quản lý.

Bạn có thể truy cập `Dashboard` **(URL)** để xem trực quan đồ thị dữ liệu của các chỉ số thời tiết. Bên cạnh, bạn có thể dùng `API` do trạm cung cấp để lấy về các dữ liệu thời tiết, phục vụ cho nhu cầu nghiên cứu phát triển của riêng bạn.

## Danh sách các cảm biến sử dụng

> - Cảm Biến <u>Độ Ẩm, Nhiệt Độ Không Khí</u> [`AHT20`](https://hshop.vn/products/cam-bien-do-am-nhiet-do-khong-khi-aht20-temperature-humidity-sensor).
>   - Giao thức **I2C**.
>   - Phạm vi đo nhiệt độ **-40~85 độ ºC**.
>   - Sai số đo nhiệt độ **0,3 độ ºC**.
>   - Phạm vi đo độ ẩm **0~100% RH**.
>   - Sai số đo độ ẩm **2% RH**.
>
> - Cảm Biến <u>Áp Suất Không Khí</u> [`BMP180`](https://hshop.vn/products/cam-bien-ap-suat-khong-khi-bmp180).
>   - Giao thức **I2C**.
>   - Độ nhiễu **0.02hPa (17cm)**.
>   - Phạm vi đo **300hPa ~ 1100hPa (+9000m đến -500m)**.
>
> - Cảm Biến <u>Tia Cực Tím UV</u> [`ML8511`](https://hshop.vn/products/cam-bien-anh-sang-uvm-30a).
>   - Giao thức **Analog**.
>   - Đặc tính: đo được tia **UV-A** và **UV-B**, nhạy cảm nhất với ánh sáng có bước sóng **365nm**.
>
> - Cảm Biến <u>Bụi PM2.5</u> [`PMS7003`](https://hshop.vn/products/cam-bien-bui-laser-optical-dust-sensor-pm2-5-plantower-pms7003).
>   - Giao thức **UART**.
>   - Độ phân giải **1 μg/m³**.
>   - Phạm vi đo **0~500 μg/m³**.
>   - Thời gian phản hồi **< 1s**.
>
> ---
>
> Các chỉ số môi trường sẽ đo được:
>
> - Nhiệt độ **(ºC)**.
> - Độ ẩm **(% RH)**.
> - Áp suất ko khí **(hPa)**.
> - UV λ365nm **(mW/cm<sup>2</sup>)**.
> - Bụi PM2.5 **(μg/m<sup>3</sup>)**.

## Danh sách các thiết bị phần cứng sử dụng

`ESP32` có vai trò làm <u>Trạm thời tiết</u>, nó có nhiệm vụ đọc dữ liệu từ các cảm biến và gửi lên **Cloud (Firebase)** theo một tần suất cố định.

> - [Mạch Vietduino Wifi BLE ESP32](https://hshop.vn/products/mach-vietduino-wifi-ble-esp32-arduino-compatible).
> - [Mạch MakerEdu Shield For Vietduino](https://hshop.vn/products/arduino-makeredu-shield).

`!` Ở những bản nâng cấp sau, `ESP32` sẽ kiêm thêm vai trò ***"Gateway"*** cho **LoRa**. Nó sẽ kiểm tra liên tục kênh **LoRa** xem có bất kì gói tin dữ liệu nào gửi đến hay ko? Nếu có thì nó sẽ xử lý phân tích gói tin đó có hợp lệ hay ko? Nếu hợp lệ, với các dữ liệu trích xuất được, nó sẽ gửi lên **Cloud (Firebase)**.

> - [Mạch Thu Phát RF SPI Lora SX1278 433MHz Ra-02 Ai-Thinker Breakout](https://hshop.vn/products/mach-thu-phat-rf-spi-lora-sx1278-433mhz-ra-02-dip).
> - hoặc
> - [Mạch Thu Phát RF SPI Lora SX1276 868/915MHz Ra-01H Ai-Thinker Breakout](https://hshop.vn/products/mach-thu-phat-rf-spi-lora-sx1276-868-915mhz-ra-01h-ai-thinker-breakout).

Tất cả thông số dữ liệu thời tiết có thể xem trực tiếp trên màn hình Oled.

> - [Màn Hình Oled 1.3 Inch Giao Tiếp I2C](https://hshop.vn/products/lcd-oled-trang-1-3-inch-giao-tiep-i2c).
>
> <u>Lưu ý</u>:
> - Driver màn 1.3 là `SH1106`.
> - Driver màn 0.96 là `SSD1306`.

## Các thành phần trong hệ thống Trạm thời tiết này

> |Tên|Vai trò|Chức năng|
> |---|-------|---------|
> |ESP32|Trạm thời tiết + Gateway|- Gửi dữ liệu lên Cloud (Firebase).<br>- Nhận dữ liệu từ LoRa (nếu có).
> |Firebase|Cloud|- Lưu trữ cơ sở dữ liệu.
> |Web|Dashboard|- Giao diện đồ họa thường cung cấp cái nhìn tổng quan về các chỉ số hiệu suất chính.
> |RESTful API|API|- Thiết kế API cho các ứng dụng web (thiết kế Web services) để tiện cho việc quản lý các resource.

## Cấu hình Firebase

> - Project Name: ***"WeatherStation-of-MakerLab"***.
> - Database URL: https://weatherstation-of-makerlab-default-rtdb.asia-southeast1.firebasedatabase.app/.
> - API key: `AIzaSyBzro27svhQQF076n2IN1asR_B2V6D92bw`.
>
> Cấu trúc **Realtime Database:**
>
> |Key|Value|Unit|Mô tả|
> |---|-----|----|-----|
> |pm1|"number"|**(μg/m<sup>3</sup>)**|- Mật độ bụi PM1.0
> |pm2d5|"number"|**(μg/m<sup>3</sup>)**|- Mật độ bụi PM2.5
> |pm10|"number"|**(μg/m<sup>3</sup>)**|- Mật độ bụi PM10
> |humi|"number"|**(%RH)**|- Độ ẩm không khí
> |temp|"number"|**(ºC)**|- Nhiệt độ không khí
> |pres|"number"|**(Pa)**|- Áp suất
> |alti|"number"|**(m)**|- Độ cao
> |presSea|"number"|**(Pa)**|- Áp suất so với mực nước biển
> |altiSea|"number"|**(m)**|- Độ cao so với mực nước biển
> |uv|"number"|**(mW/cm<sup>2</sup>)**|- Cường độ UV
> |time|"string"|**YYYY`-`MM`-`DD`T`HH`:`MM`:`SS`Z`**|- Timestamp

## Nguồn tài liệu

**<u>Firebase</u>:**
- Hướng dẫn: [ESP32: Getting Started with Firebase (Realtime Database)](https://randomnerdtutorials.com/esp32-firebase-realtime-database/).
- Thư viện: [Firebase-ESP-Client](https://github.com/mobizt/Firebase-ESP-Client).

**<u>NTP Client</u>:**
- Hướng dẫn: [Getting Date and Time with ESP32 on Arduino IDE (NTP Client)](https://randomnerdtutorials.com/esp32-ntp-client-date-time-arduino-ide/).

**<u>OLED</u>:**
- Hướng dẫn: [Demo 6: How to use Arduino ESP32 to display information on OLED](https://www.iotsharing.com/2017/05/how-to-use-arduino-esp32-to-display-oled.html).
- Thư viện chính: [esp32-sh1106-oled](https://github.com/nhatuan84/esp32-sh1106-oled).
- Thư viện hỗ trợ: [Adafruit-GFX-Library](https://github.com/adafruit/Adafruit-GFX-Library).
- Lưu ý: cặp chân `I2C` của **ESP32 WROOM 32E**.
  - `SCL` - **GPIO22**.
  - `SDA` - **GPIO21**.
  - Địa chỉ I2C mặc định là `0x3C`.

**<u>AHT20</u>:**
- Thư viện chính: [Adafruit_AHTX0](https://github.com/adafruit/Adafruit_AHTX0/tree/master).
- Thư viện hỗ trợ: [Adafruit_BusIO](https://github.com/adafruit/Adafruit_BusIO).
- Thư viện hỗ trợ: [Adafruit_Sensor](https://github.com/adafruit/Adafruit_Sensor).
- Lưu ý: cặp chân `I2C` của **ESP32 WROOM 32E**.
  - `SCL` - **GPIO22**.
  - `SDA` - **GPIO21**.
  - Địa chỉ I2C mặc định là `0x38`.

**<u>BMP180</u>:**
- Hướng dẫn: [ESP32 with BMP180 Barometric Sensor – Guide](https://randomnerdtutorials.com/esp32-with-bmp180-barometric-sensor/).
- Thư viện chính: [Adafruit-BMP085-Library](https://github.com/adafruit/Adafruit-BMP085-Library).
- Thư viện hỗ trợ: [Adafruit_BusIO](https://github.com/adafruit/Adafruit_BusIO).
- Lưu ý: cặp chân `I2C` của **ESP32 WROOM 32E**.
  - `SCL` - **GPIO22**.
  - `SDA` - **GPIO21**.
  - Địa chỉ I2C mặc định là `0x77`.

**<u>ML8511</u>:**
- Hướng dẫn: []().
- Thư viện: []().

**<u>PMS7003</u>:**
- Thư viện: [Plantower_PMS7003](https://github.com/jmstriegel/Plantower_PMS7003).
- Lưu ý: cặp chân `UART2` của **ESP32 WROOM 32E**.
  - `RX2` - **GPIO16**.
  - `TX2` - **GPIO17**.

**<u>UART ESP32</u>:**
- Hướng dẫn: [UART của ESP32](https://deviot.vn/tutorials/esp32.66047996/esp32-uart.38331059).
- Lưu ý, ESP32 có tất cả `3` UART.
  - `Serial.begin()` ứng với `UART0` dùng để Debug.
    - `RX0` - **GPIO3**.
    - `TX0` - **GPIO1**.
  - `Serial1.begin()` ứng với `UART1`.
    - `RX1` - **GPIO9**.
    - `TX1` - **GPIO10**.
  - `Serial2.begin()` ứng với `UART2`
    - `RX2` - **GPIO16**.
    - `TX2` - **GPIO17**.

## Video dự án

> `(1)` - Test lần thứ nhất.
>
> [Test Weather Station (1)](https://www.youtube.com/watch?v=a2Sgui6cRYk).