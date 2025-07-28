/*
ESPNow auto connection test:
This is the "Server" side code
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
int chan = 1;
int paired = false;
uint8_t clientMac[6];  // This is used to store the client's mac address
uint8_t serverMac[6];  // This is our own mac address

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
  // if (data[1] == 0) return;  // Don't process message if its from the server itself
  uint8_t msgType = data[0];
  if (msgType == PAIRING){
    memcpy(&pairingData, data, sizeof(PairingMessage));
    Serial.print("Pairing request from: "); printMAC(mac);
    memcpy(clientMac, mac, 6);

    WiFi.softAPmacAddress(pairingData.macAddr);  // Overwrite with our own MAC
    pairingData.channel = chan;
    pairingData.id = 0;  // Server is ID 0
    addPeer(clientMac);
    esp_now_send(clientMac, (uint8_t*)&pairingData, sizeof(pairingData));
    paired = true;
  } else if (msgType == DATA) {
    memcpy(&dataMsg, data, sizeof(DataMessage));
    Serial.print("Received a packet: ");
    Serial.println(dataMsg.myData);
  }
}

void setup() {
  // Init Serial Monnitor
  Serial.begin(115200);
  delay(3000);

  // Init Wi-Fi
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP("esp-server", nullptr, chan); // Optional, just to enable softAP mode
  chan = WiFi.channel();
  WiFi.softAPmacAddress(serverMac);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }
  esp_now_register_recv_cb(onRecv);  // Set up callback when data is received.
}

void loop() {
  if (!paired) {
    Serial.println("Waiting to be paired...");
    delay(2000);
  } else {
  delay(10); 
  }
}