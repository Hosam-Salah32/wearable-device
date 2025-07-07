# ESP32 Dual MAX30105 Health Monitor with OLED Display, RemoteXY, and WhatsApp Alerts

## Overview

This project is an ESP32-based health monitoring system that measures:

- Heart Rate (HR)
- Blood Pressure (SBP/DBP)
- Blood Oxygen Saturation (SpO2)
- Body Temperature

The system uses two MAX30105 sensors to calculate Pulse Transit Time (PTT) and estimate blood pressure. It visualizes live data on an OLED screen, syncs with RemoteXY for cloud control, sends readings to Google Sheets, and triggers WhatsApp alerts when necessary.

## Hardware Requirements

- ESP32 board  
- 2 × MAX30105 sensors  
- OLED display (SSD1306, 128x64, I2C)  
- Push button  
- Resistors (if needed)  
- Power source  
- Breadboard and jumper wires

## Libraries Used

Ensure you have the following Arduino libraries installed:

- `WiFi.h`  
- `RemoteXY`  
- `Wire.h`  
- `MAX30105.h`  
- `PeakDetection.h`  
- `Adafruit_GFX.h`  
- `Adafruit_SSD1306.h`  
- `OLED_SSD1306_Chart.h`  
- `HTTPClient.h`  
- `spo2_algorithm.h`

## Features

- 📈 **Live Graph Display**: IR sensor signals are displayed on the OLED in real time.  
- 🩺 **Dual-Sensor PTT Estimation**: Uses peak detection from both sensors to compute PTT and estimate SBP/DBP.  
- 🌡️ **Temperature Monitoring**: Averages temperature readings from both sensors.  
- 📶 **Wi-Fi Sync**: Connects to the internet for data logging and remote control.  
- 🟢 **RemoteXY Cloud Interface**: Monitor values remotely.  
- 📤 **Google Sheets Logging**: Sends readings to a Google Sheet using Google Apps Script.  
- 📲 **WhatsApp Alerts**: Sends health status notifications through CallMeBot API.

## Configuration

### Wi-Fi
Update your credentials:
```cpp
char ssid[] = "Your_WiFi_Name";
char pass[] = "Your_WiFi_Password";
````

### RemoteXY

Replace with your own token and settings:

```cpp
#define REMOTEXY_WIFI_SSID "Your_WiFi"
#define REMOTEXY_WIFI_PASSWORD "Your_WiFi_Password"
#define REMOTEXY_CLOUD_SERVER "cloud.remotexy.com"
#define REMOTEXY_CLOUD_PORT 6376
#define REMOTEXY_CLOUD_TOKEN "Your_RemoteXY_Token"
```

### Google Sheets

Get your script ID from the published Google Apps Script:

```cpp
String GAS_ID = "Your_Google_Script_ID";
```

### WhatsApp Alert via CallMeBot

Get your API key from [https://www.callmebot.com](https://www.callmebot.com):

```cpp
String apiKey = "Your_API_Key";
String phone_number = "+Your_WhatsApp_Number";
```

## Usage

1. Upload the code to the ESP32 using Arduino IDE.
2. Power on the device and wait for the OLED to display connection status.
3. Wear the sensors — the system will align and check signal strength.
4. Press the button to start data acquisition.
5. Readings will be:

   * Displayed on OLED
   * Uploaded to Google Sheets
   * Sent via WhatsApp
   * Synced to RemoteXY cloud

## Sleep Mode

The ESP32 enters deep sleep after a set time (`wake_time`), and wakes on:

* Capacitive touch pad event
* Timer

Adjust sleep settings here:

```cpp
int SleepingTime = 30; // seconds
int wake_time = 2; // minutes until forced sleep
```

## Notes

* Blood pressure estimation is **not medically certified**.
* PTT-based BP estimation can vary with placement and signal strength.
* Ensure proper finger placement on sensors for accurate results.

## Credits

Developed by: Hosam Salah
Powered by: ESP32, MAX30105, and CallMeBot API
