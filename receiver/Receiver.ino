#include <esp_now.h>
#include <WiFi.h>

// Shared secret
#define SHARED_SECRET  0xBEEF

typedef struct {
  uint16_t secret;
  uint16_t seq;
  int value;
} Message;

uint16_t lastSeq = 0;
int acceptedCount = 0;
int rejectedCount = 0;

void printMac(const uint8_t *mac) {
  char macStr[18];
  snprintf(macStr, 18, "%02X:%02X:%02X:%02X:%02X:%02X",
      mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.print(macStr);
}

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  Message msg;
  memcpy(&msg, incomingData, sizeof(msg));

  Serial.print("Recv from ");
  printMac(mac);
  Serial.print(" seq=");
  Serial.print(msg.seq);
  Serial.print(" val=");
  Serial.print(msg.value);

  // Check shared secret
  if (msg.secret != SHARED_SECRET) {
    Serial.println(" ⛔ wrong secret");
    rejectedCount++;
    return;
  }

  // Replay protection
  if (msg.seq <= lastSeq) {
    Serial.println(" ⛔ replay");
    rejectedCount++;
    return;
  }

  lastSeq = msg.seq;
  acceptedCount++;
  Serial.println(" ✅ accepted");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("Receiver ready");
}

void loop() {
  delay(1000);
  Serial.printf("Stats: accepted=%d rejected=%d\n", acceptedCount, rejectedCount);
}
