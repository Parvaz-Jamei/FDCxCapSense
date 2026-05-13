# FDC1004 Register Map Used by FDCxCapSense

Primary source: TI FDC1004 datasheet Rev. C. This document records only the subset implemented by this library.

## Device identity

| Register | Purpose | Expected value |
|---:|---|---:|
| `0xFE` | Manufacturer ID | `0x5449` |
| `0xFF` | Device ID | `0x1004` |

`begin()` reads both registers. If the device does not respond, it returns `DeviceNotFound`/`I2cError` path. If Device ID is wrong, it returns `InvalidDeviceId`.

## Measurement result registers

| Register | Purpose |
|---:|---|
| `0x00` | Measurement 1 MSB |
| `0x01` | Measurement 1 LSB |
| `0x02` | Measurement 2 MSB |
| `0x03` | Measurement 2 LSB |
| `0x04` | Measurement 3 MSB |
| `0x05` | Measurement 3 LSB |
| `0x06` | Measurement 4 MSB |
| `0x07` | Measurement 4 LSB |

The driver reads the MSB register first, then the LSB register. It combines the 24-bit two's-complement value as:

```text
raw24 = (MSB_register << 8) | ((LSB_register >> 8) & 0xFF)
```

The driver then sign-extends bit 23 into a 32-bit signed integer.

## Measurement configuration registers

| Register | Purpose |
|---:|---|
| `0x08` | Measurement 1 configuration |
| `0x09` | Measurement 2 configuration |
| `0x0A` | Measurement 3 configuration |
| `0x0B` | Measurement 4 configuration |

Implemented single-ended encoding:

| Field | Bits | Phase 2 behavior |
|---|---:|---|
| `CHA` | `[15:13]` | `CIN1..CIN4` encoded as `0..3` |
| `CHB` | `[12:10]` | `b111` = disabled path, `b100` = CAPDAC path |
| `CAPDAC` | `[9:5]` | `0` when disabled; user-provided code `0..31` when CAPDAC enabled |
| Reserved | `[4:0]` | Always zero |

Phase 1.1 hotfix separated these modes explicitly:

```cpp
fdc.configureSingleEnded(measurement, channel);              // CHB=b111, CAPDAC disabled
fdc.configureSingleEndedWithCapdac(measurement, channel, n); // CHB=b100, CAPDAC enabled, n=0..31
```

The three-argument `configureSingleEnded(measurement, channel, capdacCode)` remains as a backward-compatible CAPDAC-enabled alias only.

CAPDAC offset relationship documented for user interpretation:

```text
Coffset = CAPDAC_code * 3.125 pF
```

Maximum accepted `capdacCode` is `31`, corresponding to `96.875 pF` nominal offset contribution.

## FDC configuration register

| Register | Purpose |
|---:|---|
| `0x0C` | FDC configuration |

Implemented fields:

| Field | Bits | Phase 2 behavior |
|---|---:|---|
| `RATE` | `[11:10]` | `b01` = 100 S/s, `b10` = 200 S/s, `b11` = 400 S/s |
| `REPEAT` | `[8]` | Not enabled by helper API |
| `MEAS_1..4` | `[7:4]` | Single-shot trigger bit |
| `DONE_1..4` | `[3:0]` | Polled by `waitForMeasurement()` |

## Conversion formula

The library uses the documented relationship:

```text
capacitance_pf = raw24 / 2^19 + Coffset
```

`Coffset` is applied only when the measurement was configured through the CAPDAC-enabled path.

## Reserved register policy

Registers `0x15` through `0xFD` are treated as reserved. The library blocks writes to unknown/reserved/read-only registers via `FDCxStatus::InvalidArgument`.
