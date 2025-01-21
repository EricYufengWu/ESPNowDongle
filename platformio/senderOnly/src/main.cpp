/*
 * Modified from the following author's creation by yufeng.wu0902@gmail.com.
 * "THE BEER-WARE LICENSE" (Revision 42):
 * regenbogencode@gmail.com wrote this file. As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you
 * think this stuff is worth it, you can buy me a beer in return
 */
#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

// CHANGE THIS TO YOUR RECEIVER'S MAC ADDRESS
uint8_t receiver_mac[] = {0x54, 0x32, 0x04, 0x86, 0xDD, 0x74};

// Structure example to send data
// Must match the receiver structure
// typedef struct struct_message {
//   float pos[8];
//   uint16_t dur;
//   bool torque = false;
// } struct_message;
// If using struct, create a struct_message called myData
// struct_message myData;

// Otherwise simple create a char array.
char myData[100];

// String for storing serial command
String cmd;

// peerInfo
esp_now_peer_info_t peerInfo;

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  // Serial.print("\r\nLast Packet Send Status:\t");
  // // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  // Serial.println(!status);
}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  Serial.setTimeout(100);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);

  // Register peer
  memcpy(peerInfo.peer_addr, receiver_mac, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  if(Serial.available()){
    // Read until new line character and store in myData
    int bytesRead = Serial.readBytesUntil('\n', myData, sizeof(myData) - 1);  // Read until newline or buffer size limit

    if (bytesRead > 0) {
      myData[bytesRead] = '\0';  // Null-terminate the string
      Serial.print("Received input from serial monitor: ");
      Serial.println(myData);  // Print received data
    }

    // Send to receiver's mac address
    esp_err_t result = esp_now_send(receiver_mac, (uint8_t *) &myData, 
                                    sizeof(myData));
  }
  delay(10);
}
