#include <Arduino.h>
#include <WiFiManager.h> // Include the Wi-FiManager library
#include <NTPClient.h> // Include the NTPClient library
#include <WiFiUdp.h> // Include the WiFiUdp library
#include <DS3232RTC.h>

DS3232RTC RTC;

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
  // Initialize the RTC
  RTC.begin();

  autoConnectToWiFi();

  // Initialize the time client
  timeClient.begin();

  // Set the system time using NTP
  timeClient.update();
  RTC.set(timeClient.getEpochTime()); // Set the RTC time, 
  // getEpochTime関数は、NTPClientクラスのメンバ関数。この関数は、NTPサーバーから取得したエポック時間（Unix時間）を返します。
}

void loop() {
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis;
  // Print the current time
  tmElements_t tm;
  RTC.read(tm);
  Serial.print(tm.Year + 1970, DEC);
  Serial.print("/");
  Serial.print(tm.Month, DEC);
  Serial.print("/");
  Serial.print(tm.Day, DEC);
  Serial.print(" ");
  Serial.print(tm.Hour, DEC);
  Serial.print(":");
  Serial.print(tm.Minute, DEC);
  Serial.print(":");
  Serial.println(tm.Second, DEC);
  }
}