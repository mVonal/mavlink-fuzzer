# MAVLink Fuzzer

A black-box, mutation-based network fuzzer targeting the MAVLink v2 protocol, tested against ArduPilot SITL (Software In The Loop).

Built as a security research tool to explore attack surfaces in UAV communication protocols — specifically how autopilot systems handle malformed or unexpected MAVLink packets.

---

## What This Project Does

UAVs (Unmanned Aerial Vehicles) communicate with Ground Control Stations (GCS) using the MAVLink protocol. This fuzzer:

1. Captures a real MAVLink v2 packet from a live SITL session (seed)
2. Applies mutation strategies to generate malformed variants
3. Sends them directly to the ArduPilot SITL over UDP (bypassing MAVProxy)
4. Logs every packet sent with full hex dump and mutation description

The goal is to test how ArduPilot's MAVLink parser responds to boundary conditions, invalid field values, and unexpected packet structures — findings that are relevant to real-world UAV security assessments.

---

## Architecture

```
┌─────────────────────────────────────────────────┐
│                  mavlink-fuzzer                  │
│                                                  │
│  main.c          ← orchestrates the fuzz loop   │
│  mavlink_types.h ← MAVLink v2 header struct (C) │
│  parser.c        ← byte-level packet parser      │
│  mutator.c/h     ← mutation engine (3 strategies)│
│  sender.c/h      ← UDP socket layer              │
│  logger.c/h      ← structured log with hex dumps │
└──────────────────────┬──────────────────────────┘
                       │ UDP :5501
                       ▼
              ┌─────────────────┐
              │  ArduPilot SITL │
              │  (arducopter)   │
              └─────────────────┘
```

### Mutation Strategies

| Strategy | Description | Security Relevance |
|---|---|---|
| **Bit Flip** | Flips a random bit in a random byte | Simulates EMI / memory corruption |
| **Boundary Value** | Replaces a byte with 0x00, 0xFF, 0x7F, 0x80 | Triggers integer overflow/underflow edge cases |
| **Length Corruption** | Sets `len` field to 0 or 255 | Tests parser's trust in the length field (buffer over-read risk) |

---

## Key Technical Concepts

- **MAVLink v2 Header Parsing**: Implemented as a packed C struct (`__attribute__((packed))`) to ensure byte-exact alignment with the wire format
- **Black-box Fuzzing**: No instrumentation of the target — behavior observed externally via ArduPilot's MAV Errors counter and STATUSTEXT messages
- **Seed Corpus**: Real packets captured from SITL using a custom Python UDP sniffer, then used as mutation base
- **Reproducibility**: Every sent packet is logged with its full hex representation — any anomaly can be replayed exactly

---

## Project Structure

```
mavlink-fuzzer/
├── src/
│   ├── mavlink_types.h    # MAVLink v2 header struct definition
│   ├── parser.c           # Header parser with magic byte validation
│   ├── mutator.h          # Mutation strategy interface
│   ├── mutator.c          # Bit flip, boundary value, length corruption
│   ├── sender.h           # UDP sender interface
│   ├── sender.c           # UDP socket implementation
│   ├── logger.h           # Logger interface
│   └── logger.c           # Timestamped structured log with hex dumps
├── logs/
│   └── .gitkeep           # Log output directory (fuzzer_results.log goes here)
├── Makefile
└── README.md
```

---

## Build & Run

### Requirements

- Linux (tested on Kali Linux 2024.x)
- GCC
- ArduPilot SITL running locally

### Build

```bash
git clone https://github.com/YOUR_USERNAME/mavlink-fuzzer.git
cd mavlink-fuzzer
make
```

### Start ArduPilot SITL (separate terminal)

```bash
cd ~/ardupilot/ArduCopter
sim_vehicle.py --console
```

Wait until you see `ArduPilot Ready` in the SITL output.

### Run the Fuzzer

```bash
./mavlink_fuzzer
```

Output example:

```
=== MAVLink Fuzzer v0.1 ===
Hedef : 127.0.0.1:5501
Seed  : 20 byte HEARTBEAT

[SENDER] UDP socket hazir -> 127.0.0.1:5501
--- FUZZ #1 | BitFlip byte[3] bit[6] ---
Magic:          0xFD
Payload Length: 9
...
Durum: GONDERILDI
```

### View Logs

```bash
cat fuzzer_results.log
```

Log format:

```
[2025-07-01 14:32:01] FUZZ #1
  Mutation : BitFlip byte[3] bit[6]
  Durum    : GONDERILDI
  Hex      : fd 09 00 40 e4 01 01 00 00 00 00 00 00 00 02 03 51 03 96 43
```

---

## Observations from Testing

- ArduPilot silently discards packets with invalid magic bytes — no error counter increment
- Length field corruption (`len=0` or `len=255`) causes MAVProxy-level rejection when routing through port 14550, but passes through when sent directly to SITL port 5501
- Component ID spoofing (e.g., `comp_id=0xFF`) is accepted without error — no authentication at the protocol level in MAVLink v2 without signing enabled
- Message ID mutations (e.g., ID 0→4194304) are silently dropped — ArduPilot does not log unknown message IDs to the MAV Errors counter

---

## Security Context

This project is a practical exploration of UAV protocol attack surfaces, relevant to:

- **Embedded systems pentesting**: Understanding how autopilot software handles malformed input at the protocol layer
- **UAV security research**: MAVLink v1 (no signing) and MAVLink v2 with signing disabled are widely deployed — this fuzzer demonstrates the risk of unauthenticated command injection
- **Offensive security tooling**: The mutation engine and UDP sender form the basis of a protocol-aware fuzzer that can be extended to other embedded protocols (CAN bus, UAVCAN, etc.)

---

## Future Work

- [ ] Add response monitoring (detect ArduPilot hang via heartbeat timeout)
- [ ] Implement command injection mutations (ARM/DISARM spoofing)
- [ ] Add seed corpus from multiple message types (not just HEARTBEAT)
- [ ] Integrate with coverage-guided fuzzing (AFL++ with QEMU mode)
- [ ] Test against PX4 SITL for cross-implementation comparison

---

## References

- [MAVLink v2 Serialization](https://mavlink.io/en/guide/serialization.html)
- [ArduPilot SITL Documentation](https://ardupilot.org/dev/docs/sitl-simulator-software-in-the-loop.html)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet)
- [MAVLink Security](https://mavlink.io/en/guide/security.html)
