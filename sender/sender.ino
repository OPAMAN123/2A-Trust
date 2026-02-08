// ======================= SENDER =======================
#include <WiFi.h>
#include <esp_now.h>
#include "mbedtls/md.h"

/* ------------ CONFIG ------------ */
uint8_t receiverMAC[] = {0xB0,0xCB,0xD8,0x9D,0x33,0x14};
#define SEND_INTERVAL_MS 2000

/* ------------ SHARED KEY ------------ */
static const uint8_t SHARED_KEY[32] = {
  0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,
  0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00,
  0x10,0x20,0x30,0x40,0x50,0x60,0x70,0x80,
  0x90,0xA0,0xB0,0xC0,0xD0,0xE0,0xF0,0x01
};

/* ------------ DATA STRUCTS ------------ */
typedef struct {
  uint32_t msg_id;
  char payload[32];
  uint32_t timestamp_ms;
  uint8_t auth_tag[8];
} emergency_packet_t;

typedef struct {
  uint32_t msg_id;
  uint32_t ack_timestamp_ms;
} ack_packet_t;

/* ------------ STATE ------------ */
uint32_t seq = 1;
uint32_t sendTimes[1024];
unsigned long lastSend = 0;
uint32_t sent = 0, acked = 0;

/* ------------ HELPERS ------------ */
void compute_hmac8(uint8_t *data, size_t len, uint8_t out[8]) {
  uint8_t full[32];
  const mbedtls_md_info_t *md = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
  mbedtls_md_hmac(md, SHARED_KEY, sizeof(SHARED_KEY), data, len, full);
  memcpy(out, full, 8);
}

/* ------------ CALLBACK ------------ */
void onReceive(const esp_now_recv_info_t *info,
               const uint8_t *data, int len) {

  ack_packet_t ack;
  memcpy(&ack, data, sizeof(ack));
  uint32_t rtt = millis() - sendTimes[ack.msg_id % 1024];
  acked++;

  Serial.print("ACK,");
  Serial.print(ack.msg_id);
  Serial.print(",");
  Serial.println(rtt);
}

/* ------------ SETUP ------------ */
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  esp_now_init();

  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, receiverMAC, 6);
  esp_now_add_peer(&peer);

  esp_now_register_recv_cb(onReceive);
  Serial.println("SENDER_READY");
}

/* ------------ LOOP ------------ */
void loop() {
  if (millis() - lastSend < SEND_INTERVAL_MS) return;
  lastSend = millis();

  emergency_packet_t pkt;
  pkt.msg_id = seq;
  strcpy(pkt.payload, "HELP");
  pkt.timestamp_ms = millis();

  uint8_t buf[4 + 32 + 4];
  memcpy(buf, &pkt.msg_id, 4);
  memcpy(buf + 4, pkt.payload, 32);
  memcpy(buf + 36, &pkt.timestamp_ms, 4);

  compute_hmac8(buf, sizeof(buf), pkt.auth_tag);

  sendTimes[seq % 1024] = pkt.timestamp_ms;
  esp_now_send(receiverMAC, (uint8_t*)&pkt, sizeof(pkt));

  Serial.print("SEND,");
  Serial.println(seq);

  seq++;
  sent++;
}

