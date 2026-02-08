# 2A-Trust — Beginner-Friendly ESP-NOW Authentication Extension

## Project Overview

2A-Trust adds simple **lightweight authentication and replay protection** to an existing ESP-NOW communication system using ESP32 boards with Arduino IDE.

This project focuses on **practical trust guarantees**, not full encryption.

## Design Goals

- Only accept packets from a trusted sender
- Reject replayed packets
- Keep implementation beginner-friendly (Arduino IDE)
- Demonstrate engineering trade-offs

## Threat Model

- Attacker can inject packets or replay old packets
- Attacker cannot extract firmware secrets
- Goal: reject invalid packets with minimal overhead

## Mechanisms

1. **Shared Secret / Token** in every packet  
2. **Monotonic Sequence Number** for replay detection

## Included Code

- `sender/sender.ino`: sender with shared secret + sequence number  
- `receiver/receiver.ino`: receiver rejecting invalid packets

## How to Use

1. Flash `sender.ino` to one ESP32
2. Flash `receiver.ino` to another
3. Open Serial Monitor on receiver
4. Observe authenticated traffic vs rejected packets

## Limitations

- No cryptographically strong authentication (no AES/HMAC)
- Shared secret in firmware can be extracted
- Only single sender-to-receiver tested

## Results

Track:
- Accepted vs Rejected packet counts
- Rejection reasons (wrong secret / replay)
- Simple performance metrics exported to `/results`

## Repository Structure



2A-Trust/

├── sender/

│ └── sender.ino

├── receiver/

│ └── receiver.ino

├── results/

│ └── *.csv

└── README.md
