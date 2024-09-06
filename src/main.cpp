#include <Arduino.h>
#include <WiFiManager.h> // Include the Wi-FiManager library
#include <NTPClient.h> // Include the NTPClient library
#include <WiFiUdp.h> // Include the WiFiUdp library
#include <DS3232RTC.h>
#include <SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Initialize the OLED display using Wire library
SSD1306Wire display(0x3c, SDA, SCL);   // ADDRESS, SDA, SCL  -  SDA and SCL usually populate automatically based on your board's pins_arduino.h e.g. https://github.com/esp8266/Arduino/blob/master/variants/nodemcu/pins_arduino.h
// SH1106Wire display(0x3c, SDA, SCL);

DS3232RTC RTC;
// Timer variables
unsigned long previousMillis = 0;
unsigned long previousNTPMillis = 0;
const unsigned long interval = 1000; // 1 second in milliseconds
const unsigned long ntpInterval = 3600000; // 60 minutes in milliseconds

// Set your NTP server credentials
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 9 * 3600; // GMT offset in seconds (JST: GMT+9)
const int   daylightOffset_sec = 0; // Daylight offset in seconds

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, gmtOffset_sec, daylightOffset_sec); 
// Create a NTPClient to get the time

void autoConnectToWiFi()
{
  // Connect to Wi-Fi using Wi-FiManager
  WiFiManager wifiManager;
  if (!wifiManager.autoConnect("AutoConnectAP"))
  {
    Serial.println("Failed to connect and hit timeout");
    delay(3000);
    ESP.restart();
  }
  Serial.println("Connected to Wi-Fi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  // Start the serial communication
  Serial.begin(115200);
  Wire.begin(4,5);
  // Initialize the RTC
  RTC.begin();

  autoConnectToWiFi();

  display.init();
  // display.flipScreenVertically();
  display.setContrast(255);
  display.clear();
  display.display();
  delay(2000); // Pause for 2 seconds
  display.setFont(ArialMT_Plain_16);
  // Initialize the time client
  timeClient.begin();
  // Set the system time using NTP
  timeClient.update();
  RTC.set(timeClient.getEpochTime()); // Set the RTC time, 
  // getEpochTime関数は、NTPClientクラスのメンバ関数。この関数は、NTPサーバーから取得したエポック時間（Unix時間）を返します。
    display.drawString(40, 0, "NTP Updated:");
    display.display();
     tmElements_t tm;
    RTC.read(tm);
      // Format the time as a string
    char timeStr[20];
    sprintf(timeStr, "%04d/%02d/%02d %02d:%02d:%02d", tmYearToCalendar(tm.Year), tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second);
      // Print the time to Serial Monitor
    Serial.println(timeStr);
}

void loop() {// Check if 1 second has passed
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
  // Print the current time
  tmElements_t tm;
  RTC.read(tm);
      // Format the time as a string
    char timeStr[20];
    sprintf(timeStr, "%04d/%02d/%02d\n%02d:%02d:%02d", tmYearToCalendar(tm.Year), tm.Month, tm.Day, tm.Hour, tm.Minute, tm.Second);
      // Print the time to Serial Monitor
    Serial.println(timeStr);
      // Display the time on OLED
    // display.clear();
    display.setColor(BLACK);
    display.fillRect(0, 20, SCREEN_WIDTH, 40); // Clear the specific area
    display.setColor(WHITE);
    display.setFont(ArialMT_Plain_16);
    display.setTextAlignment(TEXT_ALIGN_LEFT);
    display.drawString(0, 0, "Time:");
    display.drawString(0, 20, timeStr);
    display.display();
  }
    // Check if checktimer has passed
  if (currentMillis - previousNTPMillis >= ntpInterval) {
    previousNTPMillis = currentMillis;
    
    // Update the system time using NTP
    timeClient.update();
    RTC.set(timeClient.getEpochTime()); // Set the RTC time
    Serial.println("Updated the system time using NTP");
    display.drawString(40, 0, "NTP Updated:");
    display.display();
  }
}