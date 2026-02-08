#include <esp_now.h>
#include <WiFi.h>

// Shared secret
#define SHARED_SECRET  0xBEEF

typedef struct {
  uint16_t secret;
  uint16_t seq;
  int value;
} Message;

uint16_t seqCounter = 0;
uint8_t peerAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, peerAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  esp_now_add_peer(&peerInfo);

  Serial.println("Sender ready");
}

void loop() {
  Message msg;
  msg.secret = SHARED_SECRET;
  msg.seq = seqCounter++;
  msg.value = random(0, 100); // example payload

  esp_err_t result = esp_now_send(peerAddress, (uint8_t *)&msg, sizeof(msg));
  if (result == ESP_OK) {
    Serial.printf("Sent seq=%d value=%d\n", msg.seq, msg.value);
  } else {
    Serial.println("Send failed");
  }

  delay(500);
}
