## Experimental Runs

All experiments were run with identical hardware, distance, and send interval.
Each run consisted of 300 packets.

### Control Run
- Legitimate sender
- Correct shared key
- Expected: All packets accepted

### Error Run: Wrong Key
- Sender uses incorrect shared secret
- Expected: All packets rejected (BAD_AUTH)

### Error Run: Replay
- Previously accepted packets resent
- Expected: All packets rejected (REPLAY)

### Error Run: Unknown Sender
- Sender MAC not in receiver allowlist
- Expected: All packets rejected (UNKNOWN_MAC)

