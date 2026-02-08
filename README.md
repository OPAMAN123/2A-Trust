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

## How to Use

1. Flash the `sender/sender.ino` sketch onto one ESP32.
2. Flash the `receiver/receiver.ino` sketch onto another ESP32.
3. Open the Serial Monitor on the receiver.
4. Observe accepted and rejected packets.

Rejected packets are logged with reasons (e.g., wrong secret, replay).

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
