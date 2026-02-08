# 2A-Trust — Lightweight Authentication for ESP-NOW

## Overview
2A-Trust is a lightweight MAC-layer authentication and replay-protection extension
for offline ESP-NOW communication systems.

The project focuses on trust and correctness rather than encryption.

## Threat Model
- Attacker can inject and replay packets
- Attacker cannot extract firmware secrets
- Goal: accept only authentic and fresh packets

## Design
- HMAC-SHA256 (truncated to 64 bits)
- 32-bit sequence numbers
- Sliding-window replay protection (64 packets)
- Explicit accept / reject logic

## Experiments
Four experimental runs were conducted:
- Control (legitimate traffic)
- Wrong-key attack
- Replay attack
- Unknown sender attack

Each run consisted of 300 packets.

## Results
- Control: 300 / 300 accepted
- Wrong key: 300 / 300 rejected (BAD_AUTH)
- Replay: 300 / 300 rejected (REPLAY)
- Unknown sender: 300 / 300 rejected (UNKNOWN_MAC)

## Limitations
- No confidentiality (payload not encrypted)
- Pre-shared key only
- Single-sender model

## Status
Feature-frozen at v2.0

