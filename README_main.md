# SSS2 Firmware — PlatformIO Project (Teensy 3.6)

Converted from the original Arduino IDE project for use with PlatformIO.

## Project Structure

```
SSS2_PlatformIO/
├── platformio.ini              ← PlatformIO config (board, libs, flags)
├── src/
│   ├── main.cpp                ← Main firmware (was SSS2_Firmware.ino)
│   ├── SSS2_functions.h        ← Core functions header
│   ├── SSS2_board_defs_rev_5.h ← Board pin definitions (Rev 5 - default)
│   └── SSS2_board_defs_rev_3.h ← Board pin definitions (Rev 3)
└── lib/
    ├── FlexCAN_Library/        ← Teensy CAN bus library
    ├── MCP23017/               ← Adafruit MCP23017 I/O expander (i2c_t3 version)
    ├── MCP_CAN_lib/            ← MCP2515 SPI CAN libraryalternate between formated md and text in vs code
    ├── OneButton/              ← Button debounce library
    ├── ArduinoThread/          ← Thread management library
    └── TeensyID/               ← Teensy unique ID library
```

## Setup & Build

1. Install [PlatformIO IDE](https://platformio.org/install/ide?install=vscode) (VS Code extension recommended)
2. Open this folder in VS Code
3. PlatformIO will auto-install dependencies from `platformio.ini`
4. Click **Build** (✓) or run: `pio run`
5. To upload: `pio run --target upload`

## Dependencies Auto-Installed by PlatformIO

| Library | Source                   |
| ------- | ------------------------ |
| Encoder | paulstoffregen/Encoder   |
| Time    | paulstoffregen/Time      |
| i2c_t3  | github.com/nox771/i2c_t3 |

## Bundled Local Libraries (in `lib/`)

| Library         | Notes                                              |
| --------------- | -------------------------------------------------- |
| FlexCAN_Library | Teensy FlexCAN driver                              |
| MCP23017        | Modified Adafruit lib using i2c_t3 instead of Wire |
| MCP_CAN_lib     | MCP2515 SPI CAN driver                             |
| OneButton       | Single-button state machine                        |
| ArduinoThread   | Cooperative threading                              |
| TeensyID        | Reads Teensy chip UID                              |

## J1939 Proprietary A Service Interface (PGN 0xEF00)

The SSS2 exposes a remote-control service layer over the J1939 bus using **PGN 0xEF00
(Proprietary A, PDU1)**. Any tool on the bus can read or write SSS2 settings by sending an
8-byte CAN frame addressed to the target SSS2's source address (e.g. **0x80**).

The service is **always listening** — it works even when the SSS2 ignition output is off,
which allows a remote tool to turn ignition on over CAN.

---

### 1. J1939 29-bit CAN ID

J1939 uses extended (29-bit) CAN IDs. For a PDU1 message the bits are:

```
Bit 28–26   Bit 25   Bit 24   Bits 23–16   Bits 15–8   Bits 7–0
──────────  ───────  ───────  ───────────  ──────────  ────────
PRIORITY    RSVD     DP       PF (0xEF)    DA           SA
 3 bits      1 bit    1 bit    8 bits       8 bits       8 bits
```

| Field    | Meaning                     | Request value                          | Response value        |
| -------- | --------------------------- | -------------------------------------- | --------------------- |
| PRIORITY | Message priority (0=high)   | 6 (0b110)                              | 6 (0b110)             |
| DP       | Data Page                   | 0                                      | 0                     |
| PF       | PDU Format — identifies PGN | 0xEF (Proprietary A)                   | 0xEF                  |
| DA       | Destination Address         | SSS2's SA (e.g. 0x80) or 0xFF (broadcast) | requester's SA     |
| SA       | Source Address (sender)     | your tool's SA (e.g. 0x52)             | SSS2's SA (e.g. 0x80) |

**Example request CAN ID** — tool SA=0x52 targeting SSS2 SA=0x80:

```
Priority=6, DP=0, PF=0xEF, DA=0x80, SA=0x52
→ 29-bit ID = 0x18EF8052
```

**Example response CAN ID** — SSS2 (SA=0x80) replying to tool SA=0x52:

```
Priority=6, DP=0, PF=0xEF, DA=0x52, SA=0x80
→ 29-bit ID = 0x18EF5280
```

---

### 2. Payload Layout (8 bytes)

Every request and response is exactly 8 bytes:

```
Byte index:  0       1           2          3          4         5    6    7
             ┌───────┬───────────┬──────────┬──────────┬─────────┬────┬────┬────┐
Request:     │  CMD  │ SETTING # │ VALUE lo │ VALUE hi │  0xFF   │ FF │ FF │ FF │
             └───────┴───────────┴──────────┴──────────┴─────────┴────┴────┴────┘
             ┌───────┬───────────┬──────────┬──────────┬─────────┬────┬────┬────┐
Response:    │ 0x80  │ SETTING # │ VALUE lo │ VALUE hi │ STATUS  │ FF │ FF │ FF │
             └───────┴───────────┴──────────┴──────────┴─────────┴────┴────┴────┘
```

#### Byte 0 — Command (request) / Response marker

| Value  | Constant                | Meaning                        |
| ------ | ----------------------- | ------------------------------ |
| `0x01` | `J1939_SVC_SET_SETTING` | Write a setting value          |
| `0x02` | `J1939_SVC_GET_SETTING` | Read the current setting value |
| `0x80` | `J1939_SVC_RESPONSE`    | SSS2 response marker (byte 0)  |

#### Byte 1 — Setting number

An integer from **1 to 92** that selects which SSS2 hardware parameter to read or write.
This maps directly to the `settingNum` argument of `setSetting()`.
Use the `LS` serial command to list all setting names and their numbers.

#### Bytes 2–3 — Value (signed 16-bit, little-endian)

The setting value as a signed 16-bit integer, **low byte first**:

```
Byte 2 = value & 0xFF          (least-significant byte)
Byte 3 = (value >> 8) & 0xFF   (most-significant byte)
```

- In a **request**: the value to write (SET_SETTING) or `0xFF 0xFF` as don't-care (GET_SETTING).
- In a **response**: the value that is now active after the operation.

**Examples:**

| Decimal value | Byte 2 | Byte 3 |
| ------------- | ------ | ------ |
| 0             | 0x00   | 0x00   |
| 1             | 0x01   | 0x00   |
| 255           | 0xFF   | 0x00   |
| 256           | 0x00   | 0x01   |
| -1            | 0xFF   | 0xFF   |

#### Byte 4 — Status (response only; 0xFF in requests)

| Value  | Constant                 | Meaning                               |
| ------ | ------------------------ | ------------------------------------- |
| `0x00` | `J1939_SVC_STATUS_OK`    | Success                               |
| `0x01` | `J1939_SVC_IGN_REQUIRED` | Ignition is off; request blocked      |
| `0x02` | `J1939_SVC_OUT_OF_RANGE` | Setting number is 0 or ≥ 93 (invalid) |

#### Bytes 5–7 — Reserved

Always `0xFF`. Ignore on receive.

---

### 3. Ignition-Off Rules

The SSS2 enforces a whitelist when ignition is off:

| Command                 | Cold-start allowed?                |
| ----------------------- | ---------------------------------- |
| SET setting 50, value 1 | **Yes** — this turns ignition ON   |
| All other SET requests  | No — returns `IGN_REQUIRED (0x01)` |
| GET setting 50          | **Yes** — read ignition state      |
| All other GET requests  | No — returns `IGN_REQUIRED (0x01)` |

Setting **50** is the ignition relay control (0 = off, 1 = on).

---

### 4. Complete Examples

All examples use tool SA = **0x52** (82 decimal), SSS2 SA = **0x80** (example).

#### Turn ignition ON (allowed cold)

```
Request:
  CAN ID : 0x18EF8052  (extended)
  Payload: 01 32 01 00 FF FF FF FF
           ── ── ───── ── ── ── ──
           │  │  │     │  reserved
           │  │  │     └─ status (don't-care in request)
           │  │  └─── value = 0x0001 = 1  (LE: low=01 high=00)
           │  └────── setting 50 (0x32)
           └───────── CMD = SET_SETTING (0x01)

Response:
  CAN ID : 0x18EF5280  (extended)
  Payload: 80 32 01 00 00 FF FF FF
                        ──
                        status = OK (0x00) → ignition is now ON
```

#### Read ignition state (GET)

```
Request:
  CAN ID : 0x18EF8052
  Payload: 02 32 FF FF FF FF FF FF
           ── ──
           │  └─ setting 50 (0x32)
           └──── CMD = GET_SETTING (0x02)

Response:
  CAN ID : 0x18EF5280
  Payload: 80 32 01 00 00 FF FF FF
                 ─────
                 value = 1 → ignition is ON
```

#### Write a setting while ignition is ON (e.g. setting 1, value 128)

```
Request:
  CAN ID : 0x18EF8052
  Payload: 01 01 80 00 FF FF FF FF
              ── ─────
              │  value = 128 (LE: low=0x80, high=0x00)
              └─ setting 1

Response:
  CAN ID : 0x18EF5280
  Payload: 80 01 80 00 00 FF FF FF
                        ──
                        status = OK
```

#### Attempt to write while ignition is OFF (blocked)

```
Request:
  CAN ID : 0x18EF8052
  Payload: 01 01 80 00 FF FF FF FF   (setting 1, value 128)

Response:
  CAN ID : 0x18EF5280
  Payload: 80 01 00 00 01 FF FF FF
                        ──
                        status = IGN_REQUIRED (0x01)
```

#### Invalid setting number (out of range)

```
Request:
  CAN ID : 0x18EF8052
  Payload: 01 00 00 00 FF FF FF FF   (setting 0 — invalid)

Response:
  CAN ID : 0x18EF5280
  Payload: 80 00 00 00 02 FF FF FF
                        ──
                        status = OUT_OF_RANGE (0x02)
```

#### Broadcast message (DA = 0xFF)

The SSS2 also responds to broadcast requests (DA = 0xFF in the CAN ID):

```
Request CAN ID : 0x18EFFF52   (DA=0xFF = broadcast)
```

All other bytes follow the same rules as a unicast request.

---

### 5. Quick Reference Card

```
┌─────────────────────────────────────────────────────────────────┐
│             SSS2 J1939 Proprietary A — Quick Reference          │
├──────────────────────┬──────────────────────────────────────────┤
│ SSS2 Source Address  │ e.g. 0x80 (depends on unit)              │
│ PGN                  │ 0xEF00  (PF = 0xEF, PDU1)               │
│ CAN frame type       │ Extended (29-bit ID)                     │
│ Payload length       │ Always 8 bytes                           │
├──────────────────────┼──────────────────────────────────────────┤
│ CMD SET_SETTING      │ 0x01                                     │
│ CMD GET_SETTING      │ 0x02                                     │
│ Response marker      │ 0x80 (byte 0 of every response payload) │
├──────────────────────┼──────────────────────────────────────────┤
│ STATUS OK            │ 0x00                                     │
│ STATUS IGN_REQUIRED  │ 0x01                                     │
│ STATUS OUT_OF_RANGE  │ 0x02                                     │
├──────────────────────┼──────────────────────────────────────────┤
│ Setting range        │ 1 – 92 (inclusive)                       │
│ Cold-start whitelist │ SET setting 50 value 1 only              │
│ Value encoding       │ Signed int16, little-endian (bytes 2–3)  │
└──────────────────────┴──────────────────────────────────────────┘
```

---

## Changes from Original Arduino Project

- `SSS2_Firmware.ino` → `src/main.cpp` with `#include <Arduino.h>` added at top
- `SSS2_functions.h` → `#include <Arduino.h>` added at top
- All libraries moved to `lib/` (no need to install manually into Arduino libraries folder)
- `ENCODER_OPTIMIZE_INTERRUPTS` and `USE_SPI1` moved to `build_flags` in `platformio.ini`
- Added J1939 CAN message listening in `loop()` — incoming frames on CAN0 and CAN1 are parsed for J1939 Network Management (address claim) and Proprietary A service requests via `is_j1939_proprietary_a_for_us()` and `handle_j1939_service()`
- J24:17-18 default changed from CAN1 (`CAN1Switch = true`) to J1708/J1939 mode (`CAN1Switch = false`) in Rev 5 board defs, making J1939 the default protocol on that connector
- Added interrupt-safe guards (`noInterrupts()`/`interrupts()`) around all `bus.write()` calls in `loop()` to prevent ring buffer corruption with the 1ms `CANTimer` interrupt
