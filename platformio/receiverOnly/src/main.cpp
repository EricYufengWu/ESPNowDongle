/*
 * Modified from the following author's creation by yufeng.wu0902@gmail.com.
 * "THE BEER-WARE LICENSE" (Revision 42):
 * regenbogencode@gmail.com wrote this file. As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you
 * think this stuff is worth it, you can buy me a beer in return
 */
#include <Arduino.h>
#include <WiFi.h>
#include <ESPNowW.h>

// ESPNow params
char myData[100];
bool incoming = false;

void onRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  // ESPNow callback function to receive data
  // char macStr[18];
  // snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
  //          mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4],
  //          mac_addr[5]);
  // Serial.print("Last Packet Recv from: ");
  // Serial.println(macStr);
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Received a packet: ");
  Serial.println(myData);
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  WiFi.mode(WIFI_MODE_STA);
  Serial.println(WiFi.macAddress());
  WiFi.disconnect();
  ESPNow.init();
  ESPNow.reg_recv_cb(onRecv);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(10);
}