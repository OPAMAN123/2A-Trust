// ======================= RECEIVER =======================
#include <WiFi.h>
#include <esp_now.h>
#include "mbedtls/md.h"

/* ------------ CONFIG ------------ */
uint8_t senderMAC[] = {0xD4,0xE9,0xF4,0xB4,0xFF,0xF4};
#define WINDOW_SIZE 64

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

/* ------------ REPLAY STATE ------------ */
uint32_t last_seq = 0;
uint64_t replay_bitmap = 0;

/* ------------ HELPERS ------------ */
bool seq_greater(uint32_t a, uint32_t b) {
  return (int32_t)(a - b) > 0;
}

bool replay_check(uint32_t seq) {
  if (seq_greater(seq, last_seq)) {
    uint32_t shift = seq - last_seq;
    replay_bitmap = (shift >= WINDOW_SIZE) ? 1ULL : (replay_bitmap << shift) | 1ULL;
    last_seq = seq;
    return true;
  } else {
    uint32_t diff = last_seq - seq;
    if (diff >= WINDOW_SIZE) return false;
    if (replay_bitmap & (1ULL << diff)) return false;
    replay_bitmap |= (1ULL << diff);
    return true;
  }
}

void compute_hmac8(uint8_t *data, size_t len, uint8_t out[8]) {
  uint8_t full[32];
  const mbedtls_md_info_t *md = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
  mbedtls_md_hmac(md, SHARED_KEY, sizeof(SHARED_KEY), data, len, full);
  memcpy(out, full, 8);
}

/* ------------ RECEIVE CALLBACK ------------ */
void onReceive(const esp_now_recv_info_t *info,
               const uint8_t *data, int len) {

  if (len != sizeof(emergency_packet_t)) return;

  emergency_packet_t pkt;
  memcpy(&pkt, data, sizeof(pkt));

  /* MAC check */
  if (memcmp(info->src_addr, senderMAC, 6) != 0) {
    Serial.println("REJECT,UNKNOWN_MAC");
    return;
  }

  /* Replay check */
  if (!replay_check(pkt.msg_id)) {
    Serial.println("REJECT,REPLAY");
    return;
  }

  /* Auth check */
  uint8_t buf[4 + 32 + 4];
  memcpy(buf, &pkt.msg_id, 4);
  memcpy(buf + 4, pkt.payload, 32);
  memcpy(buf + 36, &pkt.timestamp_ms, 4);

  uint8_t expected[8];
  compute_hmac8(buf, sizeof(buf), expected);

  if (memcmp(expected, pkt.auth_tag, 8) != 0) {
    Serial.println("REJECT,BAD_AUTH");
    return;
  }

  /* ACCEPT */
  uint32_t now = millis();
  Serial.print("ACCEPT,");
  Serial.print(pkt.msg_id);
  Serial.print(",");
  Serial.println(now - pkt.timestamp_ms);

  ack_packet_t ack = { pkt.msg_id, now };
  esp_now_send(senderMAC, (uint8_t*)&ack, sizeof(ack));
}

/* ------------ SETUP ------------ */
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  esp_now_init();

  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, senderMAC, 6);
  esp_now_add_peer(&peer);

  esp_now_register_recv_cb(onReceive);
  Serial.println("RECEIVER_READY");
}

void loop() {}

