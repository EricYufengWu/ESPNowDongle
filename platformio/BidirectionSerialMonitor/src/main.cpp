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

// THIS IS THE MAC ADDRESS OF YOUR SENDING TARGET, NOT YOUR OWN ADDRESS!
// uint8_t receiver_mac[] = {0xEC, 0xDA, 0x3B, 0x38, 0x0C, 0x1C};
uint8_t receiver_mac[] = {0x54, 0x32, 0x04, 0x86, 0xDD, 0x74};

// Char array for sending and receiving data
char myData[100];
char theirData[100];
bool incoming = false;

// String for storing serial command
String cmd;

// peerInfo
esp_now_peer_info_t peerInfo;

// Callback when data is received
void onRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4],
           mac_addr[5]);
  memcpy(&theirData, incomingData, sizeof(theirData));
  // Serial.print("Received from ["); Serial.print(macStr); Serial.print("]: "); 
  Serial.println(theirData);
}

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  // Serial.print("\r\nLast Packet Send Status:\t");
  // // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  // Serial.println(!status);
}

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(100);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_MODE_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Receive data callback function
  ESPNow.reg_recv_cb(onRecv);

  // Register for Send CB to get the status of Trasnmitted packet
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
    // Read until new line or buffer size limit and store in myData
    int bytesRead = Serial.readBytesUntil('\n', myData, sizeof(myData) - 1);

    if (bytesRead > 0) {
      myData[bytesRead] = '\0';  // Null-terminate the string
      // Serial.print("From serial monitor: "); Serial.println(myData);
    }

    // Send to receiver's mac address
    esp_err_t result = esp_now_send(receiver_mac, (uint8_t *) &myData, 
                                    sizeof(myData));
    // Serial.print("Sent to paired device: "); Serial.println(myData);
  }
  delay(10);
}