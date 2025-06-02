# Vòng tay theo dõi sức khỏe thời gian thực

## 🩺 Mô tả dự án

Dự án thiết kế và xây dựng một **thiết bị đeo tay thông minh (vòng tay)** có khả năng đo **nhịp tim**, **nồng độ oxy trong máu (SpO2)** và **vị trí GPS theo thời gian thực**. Thiết bị giúp hỗ trợ **theo dõi sức khỏe cá nhân**, đặc biệt hữu ích cho **người cao tuổi, trẻ nhỏ** hoặc **bệnh nhân cần giám sát từ xa**.

Hệ thống ứng dụng công nghệ **IoT** để thu thập dữ liệu và hiển thị thông tin trực tiếp trên màn hình OLED, đồng thời **gửi dữ liệu lên nền tảng Thingspeak** để theo dõi từ xa.

---

## 🎯 Mục tiêu

- Theo dõi **nhịp tim** và **SpO2** của người dùng theo thời gian thực.
- Gửi **tọa độ GPS** lên server Thingspeak.
- Hiển thị thông số trực tiếp trên **màn hình OLED SSD1306**.
- Góp phần nâng cao ý thức tự chăm sóc sức khỏe cá nhân.

---

## ⚙️ Chức năng chính

- ✅ Đo **nhịp tim** và **SpO2** qua module **MAX30102**.
- ✅ Xác định vị trí theo **GPS** qua module **ATGM336H**.
- ✅ Hiển thị thông tin trên **màn hình OLED SSD1306**.
- ✅ Gửi dữ liệu thời gian thực lên **Thingspeak**.

> ⚠️ **Lưu ý**: Dự án chỉ thực hiện ở mức **nguyên mẫu thử nghiệm**, không nhằm thay thế các thiết bị y tế chuyên dụng.

---

## 🔄 Nguyên lý hoạt động

1. **Đọc nhịp tim & SpO2** từ cảm biến MAX30102 qua giao tiếp I2C.
2. **Đọc vị trí GPS** từ module ATGM336H qua giao tiếp UART.
3. **Hiển thị dữ liệu** trên màn hình OLED.
4. **Gửi dữ liệu** lên nền tảng **Thingspeak** qua WiFi.

---

## 🔌 Sơ đồ kết nối

| **Thiết bị**       | **Chân ESP8266** | **Chân Module** | **Chức năng**                         |
|--------------------|------------------|------------------|----------------------------------------|
| **GPS ATGM336H**   | D6 (GPIO12)      | TX              | UART (TX của GPS → RX ESP8266)        |
|                    | D7 (GPIO13)      | RX              | UART (RX của GPS → TX ESP8266)        |
|                    | 3.3V             | VCC             | Nguồn 3.3V                             |
|                    | GND              | GND             | Nối đất                                |
| **MAX30102**       | D1 (GPIO5)       | SCL             | I2C (xung clock)                       |
|                    | D2 (GPIO4)       | SDA             | I2C (dữ liệu)                          |
|                    | 3.3V             | VCC             | Nguồn 3.3V                             |
|                    | GND              | GND             | Nối đất                                |
| **SSD1306 OLED**   | D1 (GPIO5)       | SCL             | Dùng chung I2C với MAX30102           |
|                    | D2 (GPIO4)       | SDA             | Dùng chung I2C với MAX30102           |
|                    | 3.3V             | VCC             | Nguồn 3.3V                             |
|                    | GND              | GND             | Nối đất                                |

---

## 📡 Giao tiếp Thingspeak

- Sử dụng giao thức HTTP để **gửi dữ liệu nhịp tim, SpO2 và vị trí GPS** lên Thingspeak channel định sẵn.
- Các thông số được lưu trữ theo thời gian và có thể xem biểu đồ lịch sử.

---

## 💻 Yêu cầu phần mềm

- Arduino IDE
- Thư viện:
  - `Adafruit_SSD1306`
  - `Adafruit_GFX`
  - `Wire`
  - `MAX3010x`
  - `TinyGPS++`
  - `ESP8266WiFi`
  - `ThingSpeak`

---


## 🧪 Giới hạn và phạm vi

- Dự án **không phân tích tín hiệu sinh học chuyên sâu** như ECG/PPG.
- **Chỉ thử nghiệm ở quy mô nhỏ**, trong môi trường học tập/mô phỏng.
- **Tính khả thi đã được kiểm chứng** về mặt công nghệ cơ bản.

---

## 👨‍💻 Tác giả

Dự án được thực hiện bởi [Vũ Hoàng Giang], năm 2025.

---

## 📜 Giấy phép

MIT License
