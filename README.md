## 🩺 IoT Wearable Device for Vital Signs Monitoring

This project presents a non-invasive wearable device designed to monitor key vital signs: blood pressure, heart rate, SpO₂ (oxygen saturation), body temperature, and blood glucose levels. It uses various sensors integrated with an ESP32 microcontroller and a mobile IoT interface for real-time health monitoring.

---

### 📌 Table of Contents

- About the Project
- Motivation
- Features
- System Design
- Components Used
- Methodology
- Results
- Limitations
- Future Work

---

### 📖 About the Project

This wearable wrist-based device continuously measures:

* Blood Pressure (SBP & DBP)
* Blood Glucose Level
* Heart Rate
* SpO₂
* Body Temperature

It uses sensors such as:

* Pulse oximeter (MAX30102)
* IR temperature sensor
* IR photodiode with 940nm LED for glucose classification
* Pressure-based BP estimation (using PPG signals)

Collected data is displayed on an OLED screen and sent to a mobile application. In abnormal readings, the system triggers alerts via WhatsApp.

---

### 🚀 Motivation

Frequent vital sign monitoring is critical, especially for chronic patients. Traditional monitoring methods can be painful, time-consuming, and disruptive. This project provides a wearable, continuous, and non-invasive alternative that enhances patient comfort and self-monitoring.

---

### 🧰 Features

* Real-time data monitoring (HR, BP, Glucose, SpO₂, Temp)
* OLED display + mobile app dashboard
* Alerts via WhatsApp when readings are abnormal
* Rechargeable battery and power-efficient modes
* Historical data tracking

---

### 📐 System Design

* **Microcontroller**: ESP32 with I2C and ADC support
* **Display**: OLED screen
* **Connectivity**: Wi-Fi for IoT communication
* **Sensors**:

  * MAX30102 for SpO₂ and HR
  * Pressure circuit for BP (using PPG from wrist/finger)
  * 940nm IR sensor for blood glucose classification
  * Internal temperature sensor (via MAX30102)

---

### 🔩 Components Used

| Component                 | Purpose                        |
| ------------------------- | ------------------------------ |
| ESP32                     | Main microcontroller with WiFi |
| MAX30102                  | HR, SpO₂, Temp                 |
| IR LED 940nm + Photodiode | Blood glucose signal detection |
| Custom pressure circuit   | BP estimation using PTT        |
| OLED Display              | Local feedback                 |
| Push Button               | Trigger measurement            |
| 17350 Li-ion Battery      | Power supply                   |

---

### 📊 Methodology

* **Heart Rate**: Detected via peak detection in PPG from MAX30102.
* **Blood Pressure**: Calculated using PTT between finger and wrist PPG signals.
* **Blood Glucose**: Classified using PPG signal presence/absence from 940nm IR LED.
* **SpO₂ & Temp**: From dual-LED (red/IR) light absorption and die temperature sensor.

---

### 📈 Results

* **Blood Pressure**:

  * 106 samples from 24 individuals
  * Mean Error: SBP ≈ -2.19 mmHg, DBP ≈ -2.50 mmHg
  * Standard Deviation: SBP ≈ 10.1, DBP ≈ 9.8

* **Heart Rate**:

  * Avg. error: -3.13 BPM
  * Standard Deviation: 5.1

* **Temperature & SpO₂**:

  * Mean Error: Temp ≈ -1.20°C, SpO₂ ≈ -0.92%

* **Blood Glucose**:

  * 850nm IR ineffective
  * 940nm IR distinguished between diabetic and non-diabetic signals

---

### ⚠️ Limitations

* Small test population (24 individuals)
* Environmental factors may affect readings
* Blood glucose level detection is suitable for classification only—not precise measurement

---

### 🔮 Future Work

* Increase dataset size for better model accuracy
* Explore higher IR wavelengths (>1000nm) for quantitative glucose estimation
* Improve wearable form factor
* Add ML model on-device for anomaly detection
* Add GPS/emergency SOS functionality
