# 2A-Trust — Lightweight Authentication & Replay Protection for ESP-NOW

## Overview

2A-Trust is a beginner-friendly extension to the ESP-NOW communication protocol on ESP32 devices. It adds **simple lightweight authentication and replay protection** using a shared secret and monotonic sequence numbers.

This project prioritizes **practical trust guarantees** without heavy cryptography, making it suitable for undergraduate work in communication and embedded systems.

## Threat Model

Assumptions:
- The wireless channel is open and untrusted.
- An attacker can inject or replay packets.
- Firmware secrets are not extracted by the attacker.

Goal:
> The receiver should only accept messages from a trusted sender that contain valid fields.

## Mechanisms Implemented

1. **Shared Secret** – A fixed token included in every packet.
2. **Sequence Number** – A monotonic field to defend against replayed packets.

These mechanisms are easy to explain and implement using Arduino for ESP32.

## Packet Format

Each packet consists of:
- `secret`: 16-bit shared token
- `seq`: 16-bit sequence number
- `payload`: application data

At the receiver:
- Shared secret is checked first.
- Sequence number is checked next for freshness.

## Reproducibility Setup

### Hardware
- 2× ESP32-WROOM-32 development boards
- USB cables for programming

### Software Requirements
- Arduino IDE 1.8.19 or later
- ESP32 Arduino Core v2.0.5
- No external libraries required (ESP-NOW is built-in)

### Configuration Steps
1. Open `receiver/receiver.ino`
2. Note the receiver's MAC address printed on board or find via:
```cpp
   WiFi.macAddress()
```
3. Open `sender/sender.ino`
4. Update line 10:
```cpp
   uint8_t receiverMAC[] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};  // Your receiver MAC
```
5. Set shared secret (must match on both devices):
```cpp
   const uint16_t SHARED_SECRET = 0xABCD;  // Lines 8 (sender) and 9 (receiver)
```
6. Flash receiver first, then sender
7. Open Serial Monitor at **115200 baud**

## Limitations

- No cryptographically strong authentication.
- Payloads are not encrypted.
- Shared secret is static and visible in firmware.
- Only one sender–receiver relationship is shown.

## Evaluation

The receiver logs:
- Number of accepted packets
- Number of rejected packets
- Reason for rejection

These logs can be exported from the Serial Monitor into CSV for basic analysis.

## Security Analysis

### Shared Secret Strength
- 16-bit secret = 65,536 possible values
- Vulnerable to brute force in <1 minute with automated scanning
- **Not suitable for production**, intended for educational demonstration only

### Sequence Number Limitations
- 16-bit counter = 65,536 messages maximum
- Counter overflow requires manual reset
- No persistence across device reboots
- Sequential messages can be replayed in-order after power cycle

### Appropriate Use Cases
- Classroom demonstration of protocol validation concepts
- Low-stakes sensor networks in controlled environments
- Educational exploration of replay attack prevention

## Repository Structure

2A-Trust/

├── sender/

│ └── sender.ino

├── receiver/

│ └── receiver.ino

├── results/

│ └── *.csv
└── README.md


## Educational Value

By building and evaluating this project, you learn:
- Wireless protocol message validation
- Replay attack defenses
- Embedded engineering trade-offs
