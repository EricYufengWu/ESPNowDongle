// Compile and upload this sketch to a Seeed Studio XIAO ESP32C3 board
// Open a serial monitor in 115200 baudrate to obtain the MAC address.

#include <Arduino.h>
#include <WiFi.h>
#include <ESPNowW.h>

String MAC_ADDRESS;
// char* MAC_ADDRESS

void setup() {
  Serial.begin(115200);

  // Do not proceed unless serial port is initialized.
  while(!Serial){
    delay(100);
  }
  Serial.println("Serial port initialized.\n");
  delay(5000);

  // Get MAC address
  Serial.println("EXP32C3 MAC Address:");
  WiFi.mode(WIFI_MODE_STA);
  MAC_ADDRESS = WiFi.macAddress();
  WiFi.disconnect();
}

void loop() {
  Serial.println(MAC_ADDRESS);
  delay(5000);
}