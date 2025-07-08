# ESP32-Based Wearable Vital Signs Monitoring Device

## Overview

This project aims to develop a **wearable health monitoring device** that can non-invasively measure and analyze vital signs including:

- **Blood Pressure (SBP/DBP)**
- **Heart Rate**
- **Blood Oxygen Saturation (SpO₂)**
- **Body Temperature**
- **Blood Glucose Classification (Diabetic vs Non-Diabetic)**

The system uses an ESP32 microcontroller with various sensors and communication modules to process, display, and transmit health data in real time. Features include live OLED visualization, cloud control via RemoteXY, Google Sheets logging, and WhatsApp alerts for abnormal values.

---

## Hardware Components

- ESP32 board
- 2 × MAX30105 PPG sensors
- IR temperature sensor
- OLED Display (128x64 I2C, SSD1306)
- Push button
- Optional: Capacitive touch pad (for wake-up)
- Power supply (e.g., battery or USB)

---

## Key Features

- 📊 Real-time display of PPG signals on OLED screen
- 🧠 Blood pressure estimation using Pulse Transit Time (PTT) and dual MAX30105 sensors
- 🌡️ Continuous body temperature tracking
- 🟢 Oxygen saturation (SpO₂) measurement
- ❤️ Heart rate monitoring compared against clinical pulse oximeter readings
- 📲 Google Sheets integration via HTTP (Apps Script)
- 📶 Remote control and monitoring through RemoteXY Cloud
- 🔔 WhatsApp alerts via CallMeBot API
- 🌙 Auto sleep mode for power saving

---

## Software & Libraries

Make sure to install the following Arduino libraries:

- `MAX30105`
- `spo2_algorithm`
- `PeakDetection`
- `Wire`, `WiFi`, `HTTPClient`
- `Adafruit_GFX`, `Adafruit_SSD1306`
- `RemoteXY`
- `OLED_SSD1306_Chart`

---

## Configuration

### Wi-Fi
```cpp
char ssid[] = "Your_SSID";
char pass[] = "Your_PASSWORD";
````

### RemoteXY

```cpp
#define REMOTEXY_WIFI_SSID "Your_SSID"
#define REMOTEXY_WIFI_PASSWORD "Your_PASSWORD"
#define REMOTEXY_CLOUD_SERVER "cloud.remotexy.com"
#define REMOTEXY_CLOUD_PORT 6376
#define REMOTEXY_CLOUD_TOKEN "Your_RemoteXY_Token"
```

### Google Sheets (via Apps Script)

```cpp
String GAS_ID = "Your_Script_ID";
```

### WhatsApp Alert (via CallMeBot)

```cpp
String phone_number = "+YourPhone";
String apiKey = "YourCallMeBotAPIKey";
```

---

## Experimental Results

### 📌 Blood Pressure Estimation

* Study on **24 individuals**, with **106 samples**.
* **Mean Error**:

  * Systolic: `-2.19 mmHg` ± `10.10`
  * Diastolic: `-2.50 mmHg` ± `9.81`

### 📌 Blood Glucose Classification

* 850 nm IR LED: **Not suitable** for detecting blood glucose levels in humans.
* 940 nm IR LED:

  * **PPG detected in non-diabetics**
  * **No signal in diabetics**
* Suitable for **classification only**, not precise glucose quantification.

### 📌 Heart Rate

* 106 samples from 24 individuals.
* Compared with clinical pulse oximeter.
* **Mean Error**: `-3.13 bpm` ± `5.1`

### 📌 Body Temperature & SpO₂

* **Temperature Error**: `-1.20°C` average
* **SpO₂ Error**: `-0.92%` average

---

## Conclusion

This project successfully developed a wearable device capable of monitoring key vital signs. The device combines multiple sensors to provide real-time data on blood pressure, heart rate, SpO₂, temperature, and blood glucose classification. The use of **non-invasive** techniques ensures comfort and usability.

While the device shows promising results, limitations exist:

* **Small sample size** (24 individuals) requires expansion for broader clinical validation.
* **Environmental and physiological variability** may affect readings.

Despite these challenges, the prototype showcases the potential of wearable health technology in **empowering users** with insights into their health. With further development and integration of machine learning models, this device can contribute to **preventive healthcare** and **remote patient monitoring**.

---

## Author

Hosam Salah
📧 [hosam.s.alsayed@gmail.com](mailto:hosam.s.alsayed@gmail.com)
📍 Cairo, Egypt
