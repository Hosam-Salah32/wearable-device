#define REMOTEXY_MODE__ESP32CORE_WIFI_CLOUD
#include <WiFi.h>
#include <RemoteXY.h>
#include <Wire.h>
#include "MAX30105.h"
#include <PeakDetection.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <OLED_SSD1306_Chart.h>
#include <Adafruit_I2CDevice.h> //Include this to avoid compile errors in Platformio
#include "spo2_algorithm.h"
#include <HTTPClient.h>

String apiKey = "6321262";              //Add your Token number that bot has sent you on WhatsApp messenger
String phone_number = "+201281677152"; //Add your WhatsApp app registered phone number (same number that bot send you in url)

String url;

// Your WiFi credentials.
char ssid[] = "oth";
char pass[] = "Othman123";

// Readings: https://script.google.com/macros/s/AKfycbzeggBtb3uFd8UIe8hAGretSVLXQtRo5W4_OVOSaP4LQgwjHlmYpDOOBOVff5dD3sdltw/exec
// Readings_2: https://script.google.com/macros/s/AKfycbzwX_x1S4aVlEoQiPC92mdmWNt37Taherz94okKLuatwsWMbIIxpMQOxR5R512nu4c9JA/exec

String GAS_ID = "AKfycbzwX_x1S4aVlEoQiPC92mdmWNt37Taherz94okKLuatwsWMbIIxpMQOxR5R512nu4c9JA"; //--> spreadsheet script ID
//Your Domain name with URL path or IP address with path
const char* host = "script.google.com"; // only google.com not https://google.com
#define UPDATE_INTERVAL_HOUR  (0)
#define UPDATE_INTERVAL_MIN   (0)
#define UPDATE_INTERVAL_SEC   (0)

#define UPDATE_INTERVAL_MS    ( ((UPDATE_INTERVAL_HOUR*60*60) + (UPDATE_INTERVAL_MIN * 60) + UPDATE_INTERVAL_SEC ) * 1000 )

// RemoteXY connection settings 
#define REMOTEXY_WIFI_SSID "oth"
#define REMOTEXY_WIFI_PASSWORD "Othman123"
#define REMOTEXY_CLOUD_SERVER "cloud.remotexy.com"
#define REMOTEXY_CLOUD_PORT 6376
#define REMOTEXY_CLOUD_TOKEN "2841511695e68295f314c499136c636a"

#define I2C_SPEED_FAST 400000
#define SDA_1 21
#define SCL_1 22
#define SDA_2 33
#define SCL_2 32
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

OLED_SSD1306_Chart display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, 0x3C);
char actualThickness;

MAX30105 particleSensor;
MAX30105 particleSensor1;

PeakDetection peakDetection;
PeakDetection peakDetection1;

// Variables for both the sensors
const int stable_time = 2;
const int Acq_time = 12 - stable_time;
const int sample_sec = 100;
int sampling_time = 1000/sample_sec;
int samples_num = 0;
const int Samples = Acq_time*sample_sec;
float PTT;
float AvDelay;
int tDelay = 0;
double tPT = 0;
double PT;
float HR = 0;
float HR2 = 0;
float HRAvg = 0;
float SBP;
float DBP;
const int BUTTON_PIN = 5;
int f = 0;
int buttonState;
int flag = 0;
int Red_led = 2;

// First sensor variables 
double ir;
double filtered;
int peak;
int peaks_num = 0;
int con = 0;
double irArr[Samples];
unsigned int ir_times[Samples];
unsigned int peak_times[Acq_time*3];
unsigned int peak_times_val[Acq_time*3];

int con_num;

// Second sensor variables 
double ir1;
double filtered1;
int peak1;
int peaks_num1 = 0;
int con1 = 0;
double irArr1[Samples];
unsigned int ir1_times[Samples];
unsigned int peak1_times[Acq_time*3];
unsigned int peak1_times_val[Acq_time*3];
int con_num1;

char fdisp = 0;
float temperature;
float temperature1;
float temperatureAVG;
int BUTTON_Vcc = 19;

int usToSecConvertorFactor=1000000;   // Conversion factor for micro seconds to seconds
int SleepingTime=30;                   // Time ESP32 will go to sleep (in seconds)

int Threshold=40; // Greater the value, more the sensitivity

RTC_DATA_ATTR int bootCount = 0;
touch_pad_t touchPin;

int wake_time = 2;
void callback(){

}

// RemoteXY configurate  
#pragma pack(push, 1)
uint8_t RemoteXY_CONF[] =   // 356 bytes
  { 255,0,0,90,0,93,1,16,174,4,67,4,62,19,20,5,1,2,26,11,
  67,4,62,34,20,5,1,2,26,11,67,4,62,47,20,5,1,2,26,11,
  129,0,35,19,18,6,1,24,84,101,109,112,58,0,129,0,35,33,18,6,
  1,24,83,80,79,50,58,0,129,0,36,47,18,6,1,24,72,66,58,0,
  67,4,66,18,20,5,2,2,26,11,67,4,66,30,20,5,2,2,26,11,
  129,0,31,18,13,6,2,24,83,89,83,58,0,129,0,31,29,14,6,2,
  24,68,89,83,58,0,67,4,66,41,20,5,2,2,26,11,129,0,30,41,
  31,6,2,24,103,108,111,117,99,111,117,115,101,58,0,131,1,254,4,20,
  7,1,2,31,80,97,103,101,32,49,0,131,0,254,13,20,7,2,2,97,
  80,97,103,101,32,50,0,131,0,254,23,20,7,3,2,31,80,97,103,101,
  32,51,0,68,17,44,5,51,22,3,8,36,68,17,44,29,52,27,3,8,
  6,129,0,22,13,18,6,3,1,84,69,77,80,58,0,129,0,24,38,10,
  6,3,8,72,82,58,0,131,0,254,33,20,7,4,2,31,80,97,103,101,
  32,52,0,68,17,50,2,44,21,4,8,2,129,0,24,10,16,6,4,2,
  115,112,111,50,58,0,129,0,20,35,29,6,4,12,71,108,111,117,99,111,
  115,101,58,0,131,0,254,44,20,7,5,2,31,80,97,103,101,53,0,68,
  17,50,28,45,21,4,8,12,68,18,47,9,53,21,5,8,36,135,129,0,
  19,19,27,6,5,8,80,114,101,115,115,117,114,101,58,0 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // output variables
  char text_1[11];  // string UTF8 end zero 
  char text_2[11];  // string UTF8 end zero 
  char text_3[11];  // string UTF8 end zero 
  char text_4[11];  // string UTF8 end zero 
  char text_5[11];  // string UTF8 end zero 
  char text_6[11];  // string UTF8 end zero 
  float onlineGraph_1;
  float onlineGraph_2;
  float onlineGraph_3;
  float onlineGraph_4;
  float onlineGraph_5_var1;
  float onlineGraph_5_var2;

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0 

} RemoteXY;
#pragma pack(pop)

/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////

#if defined(AVR_ATmega328P) || defined(AVR_ATmega168) 
//Arduino Uno doesn't have enough SRAM to store 100 samples of IR led data and red led data in 32-bit format
//To solve this problem, 16-bit MSB of the sampled data will be truncated. Samples become 16-bit data.

uint16_t irBuffer[100]; //infrared LED sensor data
uint16_t redBuffer[100];  //red LED sensor data
#else
uint32_t irBuffer[100]; //infrared LED sensor data
uint32_t redBuffer[100];  //red LED sensor data
#endif

int32_t bufferLength; //data length
int32_t spo2; //SPO2 value
int8_t validSPO2; //indicator to show if  the SPO2 calculation is valid
int32_t heartRate; //heart rate value
int8_t validHeartRate; //indicator to show if the heart rate calculation is valid

// parameters to change the oximeter out and stuff
byte ledBrightness = 200; //Options: 0=Off to 255=50mA
byte sampleAverage = 8; //Options: 1, 2, 4, 8, 16, 32
byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
int sampleRate = 1600; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
int pulseWidth = 215; //Options: 69, 118, 215, 411
int adcRange = 16384; //Options: 2048, 4096, 8192, 16384

byte ledBrightness1 = 150; //Options: 0=Off to 255=50mA
byte sampleAverage1 = 8; //Options: 1, 2, 4, 8, 16, 32
byte ledMode1 = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
int sampleRate1 = 1600; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
int pulseWidth1 = 215; //Options: 69, 118, 215, 411
int adcRange1 = 16384; //Options: 2048, 4096, 8192, 16384

void update_google_sheet() {
    Serial.print("connecting to ");
    Serial.println(host);
  
    // Use WiFiClient class to create TCP connections
    WiFiClientSecure client;
    const int httpPort = 443; // 80 is for HTTP / 443 is for HTTPS!
    
    client.setInsecure(); // this is the magical line that makes everything work
    
    if (!client.connect(host, httpPort)) { //works!
      Serial.println("connection failed");
      return;
    }
       
    //----------------------------------------Processing data and sending data
    String url = "/macros/s/" + GAS_ID + "/exec";

    url += "?SBP=";
    url += String(SBP);
    
    url += "&DBP=";
    url += String(DBP);

    url += "&HRAvg=";
    url += String(HRAvg);

    url += "&temperatureAVG=";
    url += String(temperatureAVG);

    url += "&spo2=";
    url += String(spo2);
    
    Serial.print("Requesting URL: ");
    Serial.println(url);
  
    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" + 
                 "Connection: close\r\n\r\n");
  
    Serial.println();
    Serial.println("closing connection");  
}

void Read_temp_spo2(){
  RemoteXY_Handler ();
  bufferLength = 100; //buffer length of number of samples
  //read the first 100 samples, and determine the signal range
  for (byte i = 0 ; i < bufferLength ; i++)
  {
    while (particleSensor1.available() == false) //do we have new data?
      particleSensor1.check(); //Check the sensor for new data

    redBuffer[i] = particleSensor1.getRed();
    irBuffer[i] = particleSensor1.getIR();
    particleSensor1.nextSample(); //We're finished with this sample so move to next sample
  }

  //calculate heart rate and SpO2 after first 100 samples (first 4 seconds of samples)
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

  //Continuously taking samples from MAX30102.  Heart rate and SpO2 are calculated every 1 second
  RemoteXY_Handler();
  //dumping the first 25 sets of samples in the memory and shift the last 75 sets of samples to the top
  for (byte i = 25; i < 100; i++)
  {
    redBuffer[i - 25] = redBuffer[i];
    irBuffer[i - 25] = irBuffer[i];
  }

  //take 25 sets of samples before calculating the heart rate.
  for (byte i = 75; i < 100; i++)
  {
    while (particleSensor1.available() == false) //do we have new data?
      particleSensor1.check(); //Check the sensor for new data

    // digitalWrite(readLED, !digitalRead(readLED)); //Blink onboard LED with every data read

    redBuffer[i] = particleSensor1.getRed();
    irBuffer[i] = particleSensor1.getIR();
    particleSensor1.nextSample(); //We're finished with this sample so move to next sample
  }

  // After gathering 25 new samples recalculate HR and SP02
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
  
  temperature = particleSensor.readTemperature();
  temperature1 = particleSensor1.readTemperature();
  temperatureAVG = (temperature+temperature1)/2;
}

void InitializeWiFi(){
  Serial.print("Connecting");

  display.clearDisplay();
  display.setCursor(0, 10);
  display.println("Connecting...");
  display.display();
  WiFi.begin(ssid, pass); //--> Connect to your WiFi router
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(100);
  }
}

void Init_OLED(){
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay(); // clear the logo??
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setChartCoordinates(0, 60);      //Chart lower left coordinates (X, Y)
  display.setChartWidthAndHeight(123, 55); //Chart width = 123 and height = 60
  display.setXIncrement(2);                //Distance between Y points will be 5px
  display.setYLimits(ir-500, ir+500);             //Ymin = 0 and Ymax = 100
  display.setYLimitLabels("50", "100");    //Setting Y axis labels
  display.setYLabelsVisible(false);
  display.setAxisDivisionsInc(12, 6);    //Each 12 px a division will be painted in X axis and each 6px in Y axis
  display.setPlotMode(SINGLE_PLOT_MODE); //Set single plot mode
  display.setLineThickness(LIGHT_LINE);
  display.drawChart(); //Update the buffer to draw the cartesian chart
  display.display();
}

void Init_MAX_OLED(){
  Wire.begin(SDA_1, SCL_1, I2C_SPEED_FAST);
  Wire1.begin(SDA_2, SCL_2, I2C_SPEED_FAST);

  if (!particleSensor1.begin(Wire1, I2C_SPEED_FAST, 87)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX301051 was not found. Please check wiring/power. "); 
    while (1);
  }

  if (!particleSensor.begin(Wire, I2C_SPEED_FAST, 87)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. "); 
    while (1);
  }
  
  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings
  particleSensor.enableDIETEMPRDY(); //Enable the temp ready interrupt. This is required.
  peakDetection.begin(50, 1.8, 0.5);

  particleSensor1.setup(ledBrightness1, sampleAverage1, ledMode1, sampleRate1, pulseWidth1, adcRange1); //Configure sensor with these settings
  peakDetection1.begin(50, 1.8, 0.5);
}

void Init_sleepMode(){
  //Setup interrupt on Touch Pad 3 (GPIO15)
  touchAttachInterrupt(T3, callback, Threshold);
  //Configure Touchpin as wakeup source
  esp_sleep_enable_touchpad_wakeup();
  esp_sleep_enable_timer_wakeup(SleepingTime * usToSecConvertorFactor);
  Serial.println("Setup ESP32 to sleep for every " + String(SleepingTime) + " Seconds");
  //Increment boot number and print it every reboot
  bootCount=bootCount+1;
  Serial.println("Boot number: " + String(bootCount));

  if(bootCount == 1){
    Serial.println("Sleeping Zzzzz");

    display.clearDisplay();
    display.setCursor(0, 10);
    display.println("Sleeping Zzzzz");
    display.display();

    esp_deep_sleep_start();
  }
}

void Alignment(){
  int s = 0;

  ir = particleSensor.getIR();
  ir1 = particleSensor1.getIR();

  if (millis()>60000*wake_time){
    particleSensor.setup(0, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings
    particleSensor1.setup(0, sampleAverage1, ledMode1, sampleRate1, pulseWidth1, adcRange1); //Configure sensor with these settings

    display.clearDisplay();
    Serial.println("Sleeping Zzzz");
    display.setCursor(0, 55);
    display.println("Sleeping Zzzz");
    display.display(); 
    esp_deep_sleep_start();
  }

  while(ir<50000 || ir1<50000){
    if (s==0){
      Serial.println("Please wear the device");

      display.clearDisplay();
      display.setCursor(0, 10);
      display.println("Please wear the");
      display.setCursor(0, 20);
      display.println("device");
      display.display();
    
      digitalWrite(Red_led, HIGH);
      message_to_whatsapp("Please wear the device");
      s=1;
    }

    ir = particleSensor.getIR();
    ir1 = particleSensor1.getIR();
    fdisp = 0;
    if (millis()>60000*wake_time){
      particleSensor.setup(0, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings
      particleSensor1.setup(0, sampleAverage1, ledMode1, sampleRate1, pulseWidth1, adcRange1); //Configure sensor with these settings

      display.clearDisplay();
      Serial.println("Sleeping Zzzz");
      display.setCursor(0, 55);
      display.println("Sleeping Zzzz");
      display.display(); 
      esp_deep_sleep_start();
  }
  }
  digitalWrite(Red_led, LOW);
    
  if (fdisp == 0){
    display.clearDisplay(); //If chart is full, it is drawn again
    display.drawChart();
    fdisp = 1;
  }
  Serial.print(ir);
  Serial.print(", ");
  Serial.print(ir1);
  Serial.print(", ");
  Serial.println(millis());

  if (!display.updateChart(ir)) //Value between Ymin and Ymax will be added to chart 
  {
    display.setYLimits(ir-1000, ir+1000);             //Ymin = 0 and Ymax = 100
    display.clearDisplay(); //If chart is full, it is drawn again   
    display.drawChart();
  }
}

void Data_acquisition(){
  if (samples_num == 0){
    if (temperatureAVG == 0){
      display.clearDisplay();
      display.setCursor(0, 10);
    }
    else{
      display.setCursor(0, 55);
    }
    Serial.print("Aquiring data...");

    display.println("Aquiring data...");
    display.display();
  }
  while (samples_num < Samples){
    // Sensor 1   
    irArr[samples_num] = particleSensor.getIR();
    ir_times[samples_num] = millis(); 

    // Sensor 2
    irArr1[samples_num] = particleSensor1.getIR();
    ir1_times[samples_num] = millis(); 

    // Sensor 1
    peakDetection.add(irArr[samples_num]); //add this point to the window of calculations
    peak = peakDetection.getPeak(); // check if there's any peaks in the current window
    filtered = peakDetection.getFilt(); // get the filtered signal (avrage) of the current window

    // Sensor 2
    peakDetection1.add(irArr1[samples_num]); //add this point to the window of calculations
    peak1 = peakDetection1.getPeak(); // check if there's any peaks in the current window
    filtered1 = peakDetection1.getFilt(); // get the filtered signal (avrage) of the current window

    if(millis() > stable_time*1000){
      samples_num++;
    }
  }
}

void Remove_repeated_points(){
  for(int m=0; m<samples_num; m++){
    if (irArr[m] == irArr[m+32]){
      irArr[m+32] = irArr[m+31];
      Serial.println("Repeated Peak removed");
    }
    if (irArr1[m] == irArr1[m+32]){
      irArr1[m+32] = irArr1[m+31];
      Serial.println("Repeated Peak removed");
    }
  }
}

void Peak_Detection(){
    for(int i=0; i<samples_num; i++){
    // Sensor 1
    peakDetection.add(irArr[i]); //add this point to the window of calculations
    peak = peakDetection.getPeak(); // check if there's any peaks in the current window
    filtered = peakDetection.getFilt(); // get the filtered signal (avrage) of the current window

    // Sensor 2
    peakDetection1.add(irArr1[i]); //add this point to the window of calculations
    peak1 = peakDetection1.getPeak(); // check if there's any peaks in the current window
    filtered1 = peakDetection1.getFilt(); // get the filtered signal (avrage) of the current window

    // Sensor 1
    if(peak == 1 && con == 0){ 
      con = 1;
      con_num++;
    }
    else if(peak == 1 && con == 1){
      con_num++;
    }
    else if(peak < 1 && con == 1 && con_num > 10){ 
      peak_times[peaks_num] = ir_times[i];
      peaks_num++;
      con = 0;
    }

    // Sensor 2
    if(peak1 == 1 && con1 == 0){ 
      con1 = 1;
      con_num1++;
    }
    else if(peak1 == 1 && con1 == 1){
      con_num1++;
    }
    else if(peak1 < 1 && con1 == 1){ 
      peak1_times[peaks_num1] = ir1_times[i];
      peaks_num1++;
      con1 = 0;
    }
    char buffer[100]; sprintf(buffer, "%lf, %lf, %d, %d, %d, %d", irArr[i], irArr1[i], peak, peak1, ir_times[i], ir1_times[i]); Serial.println(buffer);
  }
}

void Print_peaks(){
  for (int n=0; n<peaks_num; n++){
    Serial.println(String(n) + ", " + String(peak_times[n]));
  }
  for (int n1=0; n1<peaks_num1; n1++){
    Serial.println(String(n1) + ", " + String(peak1_times[n1]));
  }
}

void False_Peaks_Remove(){
  int val_peaks = 0;
  int val_peaks1 = 0;
  if (peaks_num > peaks_num1){
    for (int k=0; k < peaks_num; k++){
      for (int l=0; l < peaks_num1; l++){
        if (peak_times[k] < peak1_times[l]+100 && peak_times[k] > peak1_times[l]-100){
          peak_times_val[val_peaks] = peak_times[k];
          val_peaks++;
          break;
        }
      }
    }
    for (int k=0; k < peaks_num1; k++){
      peak1_times_val[k] = peak1_times[k];
    }
    Serial.println("False peaks: " + String(peaks_num-val_peaks));
    peaks_num = val_peaks ;
  }
  else if(peaks_num1 > peaks_num){
    for (int k=0; k < peaks_num1; k++){
      for (int l=0; l < peaks_num; l++){
        if (peak1_times[k] < peak_times[l]+100 && peak1_times[k] > peak_times[l]-100){
          peak1_times_val[val_peaks1] = peak1_times[k];
          val_peaks1++;
          break;
        }
      }
    }
    for (int k=0; k < peaks_num; k++){
      peak_times_val[k] = peak_times[k];
    }
    Serial.println("False peaks1: " + String(peaks_num1-val_peaks1));
    peaks_num1 = val_peaks1;
  }else {
    for (int k=0; k < peaks_num; k++){
      peak_times_val[k] = peak_times[k];
    }
    for (int k=0; k < peaks_num1; k++){
      peak1_times_val[k] = peak1_times[k];
    }
  }
}

void Print_peaks_val(){
  for (int n=0; n<peaks_num; n++){
    Serial.println(String(n) + ", " + String(peak_times_val[n]));
  }
  for (int n1=0; n1<peaks_num1; n1++){
    Serial.println(String(n1) + ", " + String(peak1_times_val[n1]));
  }
}

void Calculations(){
  if (peaks_num1 != peaks_num){
    Serial.print("Peaks numbers are different"); Serial.print(", ");Serial.print(peaks_num);Serial.print(", ");Serial.println(peaks_num1);
    samples_num = 0; peaks_num = 0; peaks_num1 = 0; flag = 0;

    display.clearDisplay();
    display.setCursor(0, 10);
    display.println("Peaks are different");
    display.display();
    
    *(RemoteXY.text_4) = 'd';
    *(RemoteXY.text_4+1) = 'e';
    *(RemoteXY.text_4+2) = 'f';
    
    RemoteXY.onlineGraph_5_var1 = float (10);
    RemoteXY.onlineGraph_5_var2 = float (20);
    RemoteXY.onlineGraph_2 = float (30);
  }
  else{
    for(int j; j<peaks_num; j++){
      if(peak_times_val[j+1] != 0){
        tPT += peak_times_val[j+1] - peak_times_val[j];
      }
      tDelay += peak1_times_val[j] - peak_times_val[j];
    }

    HR = peaks_num*(60/(Acq_time));
    PT = tPT / peaks_num;
    HR2 = (60/(PT/1000));
    HRAvg = (HR+HR2)/2;
    AvDelay = tDelay / peaks_num;
    PTT = PT - AvDelay;

    if (AvDelay < 0){
      AvDelay = -1*AvDelay;
    }else if (AvDelay > 100 || HR<=0 || PTT<=0){
      Serial.print("delay is too large");
      display.clearDisplay();
      display.setCursor(0, 10);
      display.println("Delay is too large");
      flag = 0;
      return;
    }

    if(PTT >= 300 && PTT <= 499){
      SBP = 103.8-0.0777*HRAvg+0.02449*PTT;
      DBP = 55.96-0.02912*HRAvg+0.02302*PTT;
    }
    else if(PTT >= 500 && PTT <= 599){
      SBP = -1891+30.88*HRAvg+2.829*PTT-0.07119*HRAvg*HRAvg-0.03785*PTT*HRAvg+0.0004076*PTT*PTT;
      DBP = 96.77-0.589*HRAvg+0.04313*PTT;
    } 
    else if(PTT >= 600 && PTT <= 680){
      SBP = 184.3-1.329*HRAvg+0.0848*PTT;
      DBP = 96.77-0.589*HRAvg+0.04313*PTT;
    }
    else if(PTT >= 681 && PTT <= 690){
      SBP = 206.7-1.067*HRAvg-0.007432*PTT;
      DBP = 96.77-0.589*HRAvg+0.04313*PTT;
    }
    else if(PTT >= 691){
      SBP = 206.7-1.067*HRAvg-0.007432*PTT;
      DBP = 111.3-0.4669*HRAvg-0.006415*PTT;
    }
    if (SBP <= 0 || DBP<=0){
      Serial.print("delay is too large");
      display.clearDisplay();
      display.setCursor(0, 10);
      display.println("Delay is too large");
      flag = 0;
      return;
    }
    char buffer[100]; sprintf(buffer, "peaks_num: %d, HRAvg: %0.2f, AvDelay: %0.2f, PTT: %0.2f, SBP: %0.2f, DBP: %0.2f", peaks_num, HRAvg, AvDelay, PTT, SBP, DBP); Serial.println(buffer);
    samples_num = 0; peaks_num = 0; peaks_num1 = 0; tPT = 0; tDelay = 0;
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing...");

  Init_OLED();
  Init_sleepMode();
  Init_MAX_OLED();
  RemoteXY_Init (); 
  InitializeWiFi();

  pinMode(BUTTON_PIN, INPUT);
  pinMode(BUTTON_Vcc, OUTPUT); /////////////////////////////////////////////////////////////////////////////////////////////////////////
  pinMode(Red_led, OUTPUT);

  digitalWrite(BUTTON_Vcc, HIGH);
  buttonState = digitalRead(BUTTON_PIN);

  while(millis()<300){
    ir = particleSensor.getIR();
  }
  message_to_whatsapp("Ready to take readings");
}

void loop() {
  RemoteXY_Handler ();

  f = buttonState;
  buttonState = digitalRead(BUTTON_PIN);

  // print out the button's state
  if (buttonState == 1 && f == 0) {
    f = 1;
    flag = !flag;
    samples_num = 0;
  }

  if(flag == 0){
    Alignment();
  }
  else{
    Data_acquisition(); //Data Acquisition
    Remove_repeated_points();
    Peak_Detection(); // counting the peaks 
    //Print_peaks();
    False_Peaks_Remove(); // Removing flase peaks
    //Print_peaks_val();
    Calculations(); // PPT, HR, BP calculations

    if(flag == 1){
      Read_temp_spo2(); // Temp and SpO2 calculations

      Serial.println("SBP:" + String(SBP) + " DBP:" + String(DBP) + " HRavg:" + String(HRAvg) + " SpO2:" + String(spo2) + " Temp:" + String(temperatureAVG));

      display.clearDisplay();
      display.setCursor(0, 10);
      display.println("BP, HR: " + String(int(SBP)) + "/" + String(int(DBP)) + ", " + String(int(HRAvg)));
      display.setCursor(0, 25);
      display.println("SpO2: " + String(spo2));
      display.setCursor(0, 40);
      display.println("Temp: "+ String(temperatureAVG));
      display.display(); 

      dtostrf(SBP, 0, 1, RemoteXY.text_4);
      dtostrf(DBP, 0, 1, RemoteXY.text_5);
      dtostrf(HRAvg, 0, 1, RemoteXY.text_3);
      dtostrf(temperatureAVG, 0, 1, RemoteXY.text_1);
      dtostrf(spo2, 0, 1, RemoteXY.text_2);

      RemoteXY.onlineGraph_5_var1 = float (SBP);
      RemoteXY.onlineGraph_5_var2 = float (DBP);
      RemoteXY.onlineGraph_2 = float (HRAvg);
      RemoteXY.onlineGraph_1 = float (temperatureAVG);
      RemoteXY.onlineGraph_3 = float (spo2);

      update_google_sheet();

      message_to_whatsapp("BP, HR: " + String(int(SBP)) + "/" + String(int(DBP)) + ", " + String(int(HRAvg)) + ", " + "SpO2: " + String(spo2) + ", " + "Temp: "+ String(temperatureAVG));

      if (millis()>60000*wake_time){
        particleSensor.setup(0, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); //Configure sensor with these settings
        particleSensor1.setup(0, sampleAverage1, ledMode1, sampleRate1, pulseWidth1, adcRange1); //Configure sensor with these settings

        Serial.println("Sleeping Zzzz");
        display.setCursor(0, 55);
        display.println("Sleeping Zzzz");
        display.display(); 
        esp_deep_sleep_start();
      }
    }
    else{
      display.clearDisplay();
      display.display();
    }
  }
}

void  message_to_whatsapp(String message)       // user define function to send meassage to WhatsApp app
{
  //adding all number, your api key, your message into one complete url
  url = "https://api.callmebot.com/whatsapp.php?phone=" + phone_number + "&apikey=" + apiKey + "&text=" + urlencode(message);

  postData(); // calling postData to run the above-generated url once so that you will receive a message.
}

void postData()     //userDefine function used to call api(POST data)
{
  int httpCode;     // variable used to get the responce http code after calling api
  HTTPClient http;  // Declare object of class HTTPClient
  http.begin(url);  // begin the HTTPClient object with generated url
  httpCode = http.POST(url); // Finaly Post the URL with this function and it will store the http code
  if (httpCode == 200)      // Check if the responce http code is 200
  {
    Serial.println("Sent ok."); // print message sent ok message
  }
  else                      // if response HTTP code is not 200 it means there is some error.
  {
    Serial.println("Error."); // print error message.
  }
  http.end();          // After calling API end the HTTP client object.
}

String urlencode(String str)  // Function used for encoding the url
{
  String encodedString="";
  char c;
  char code0;
  char code1;
  char code2;
  for (int i =0; i < str.length(); i++){
    c=str.charAt(i);
    if (c == ' '){
      encodedString+= '+';
    } else if (isalnum(c)){
      encodedString+=c;
    } else{
      code1=(c & 0xf)+'0';
      if ((c & 0xf) >9){
          code1=(c & 0xf) - 10 + 'A';
      }
      c=(c>>4)&0xf;
      code0=c+'0';
      if (c > 9){
          code0=c - 10 + 'A';
      }
      code2='\0';
      encodedString+='%';
      encodedString+=code0;
      encodedString+=code1;
      //encodedString+=code2;
    }
    yield();
  }
  return encodedString;
}
