/*
ESPNow auto connection test:
This is the "Client" side code
 */
#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>

enum MsgType : uint8_t{
  PAIRING,
  DATA,
};

struct PairingMessage{
  uint8_t msgType;
  uint8_t id;
  uint8_t macAddr[6];
  uint8_t channel;
};

struct DataMessage{
  uint8_t msgType;
  uint8_t id;
  char myData[100];
};

// Auto pairing params
PairingMessage pairingData;
DataMessage dataMsg;
int chan = 1;  // Must be the same(similar) across server and client
bool paired = false;
uint8_t serverMac[6];
uint8_t clientMac[6];  // This device
uint8_t broadcastMAC[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
unsigned long lastPairAttempt;

void printMAC(const uint8_t* mac) {
  char buff[18];
  sprintf(buff, "%02X:%02X:%02X:%02X:%02X:%02X",
          mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.println(buff);
}

bool addPeer(const uint8_t* mac) {
  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, mac, 6);
  peer.channel = chan;
  peer.encrypt = false;
  return esp_now_add_peer(&peer) == ESP_OK;
}

void onRecv(const uint8_t* mac, const uint8_t* data, int len) {
  if (data[0] == PAIRING) {
    memcpy(&pairingData, data, sizeof(PairingMessage));
    Serial.println("Paired with server: "); printMAC(mac);
    memcpy(serverMac, mac, sizeof(serverMac));
    addPeer(serverMac);
    paired = true;
  }
}

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  Serial.setTimeout(100);
  delay(3000);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.macAddress(clientMac);
  esp_wifi_start();
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(chan, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(onRecv);  // Set up callback when data is received
  addPeer(broadcastMAC);

  // Send pairing request
  pairingData.msgType = PAIRING;
  pairingData.id = 1;
  memcpy(pairingData.macAddr, clientMac, sizeof(clientMac));
  pairingData.channel = chan;
  lastPairAttempt = millis();
}

void loop() {
  if (!paired && millis() - lastPairAttempt > 2000) {
    lastPairAttempt = millis();
    Serial.println("Sending a pairing request...");
    esp_now_send(broadcastMAC, (uint8_t*)&pairingData, sizeof(pairingData));
  } else if (Serial.available()) {
    // Read until new line character and store in myData
    int bytesRead = Serial.readBytesUntil('\n', dataMsg.myData, sizeof(dataMsg.myData) - 1);  // Read until newline or buffer size limit
    if (bytesRead > 0) {
      dataMsg.myData[bytesRead] = '\0';  // Null-terminate the string
      Serial.print("Received input from serial monitor: ");
      Serial.println(dataMsg.myData);  // Print received data
    }
    // Send to receiver's mac address
    dataMsg.msgType = DATA;
    dataMsg.id = 1;
    esp_err_t result = esp_now_send(serverMac, (uint8_t*)&dataMsg, sizeof(dataMsg));
  }
  delay(10);
}