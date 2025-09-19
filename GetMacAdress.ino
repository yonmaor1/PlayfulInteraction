#include <WiFi.h> //change if ESP8266

void setup() {
  Serial.begin(115200);
  // Delay to ensure serial monitor is open and ready
  delay(1000); 
  
  Serial.print("<<!nav>>ESP32 WROOM<<!/nav>> MAC Address: ");
  Serial.println(WiFi.macAddress());
}

void loop() {
  // Empty loop, nothing to do here
}
