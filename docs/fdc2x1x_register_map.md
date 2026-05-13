# FDC2x1x Register Map (v0.6.8 CONFIG Register Fix)

This document records the independently written, datasheet-driven register surface used by the FDC2x1x foundation. The driver is frequency-first and does not reuse FDC1004 CAPDAC or direct-pF logic.

| Address | Register group | Access policy in this library | Notes |
|---|---|---|---|
| `0x00..0x07` | `DATA_CHx` / `DATA_LSB_CHx` | Read-only | FDC2212/FDC2214 use MSB+LSB for 28-bit data. FDC2112/FDC2114 use only the 12-bit `DATA_CHx` path; `DATA_LSB_CHx` is not read. |
| `0x08..0x0B` | `RCOUNT_CHx` | Safe setter + explicit raw writer | `setRCount()` rejects public values below `0x0100`. `writeRawRCount()` is the explicit low-level escape hatch. CH2/CH3 are rejected for 2-channel variants. |
| `0x0C..0x0F` | `OFFSET_CHx` | Writable only for FDC211x where channel exists | Guarded so FDC221x does not silently accept FDC211x-specific offset writes. |
| `0x10..0x13` | `SETTLECOUNT_CHx` | Writable where channel exists | CH2/CH3 are rejected for FDC2112/FDC2212. |
| `0x14..0x17` | `CLOCK_DIVIDERS_CHx` | Safe helper + explicit raw writer | `configureClockDividers()` validates `CHx_FIN_SEL` and `CHx_FREF_DIVIDER`; `configureClockDividersForSingleEnded()` forces the single-ended `FIN_SEL=2` path; `configureClockDividersForDifferential()` requires explicit `FIN_SEL=1` or `2`. |
| `0x18` | `STATUS` | Read-only | Never writable. `waitDataReady()` requires the requested channel unread bit; `status==0` is not treated as ready. |
| `0x19` | `ERROR_CONFIG` / status configuration | Writable | Guarded as a known configuration register. |
| `0x1A` | `CONFIG` | Safe helpers recommended; raw writes validate reserved/default pattern | `setSleepMode()`, `setActiveChannel()`, `startContinuous()`, `stopConversions()`, and `setReferenceClockSource()` preserve real option bits and force the writable reserved pattern. Conversion starts by clearing `CONFIG.SLEEP_MODE_EN`. |
| `0x1B` | `MUX_CONFIG` | Safe helper recommended; raw writes validate reserved/default pattern | `setDeglitch()` writes `(existing autoscan/sequence) | 0x0208 | deglitch`; `configureSingleChannel()` writes a single-channel MUX value and clears autoscan/sequence bits. |
| `0x1C` | `RESET_DEV` | Guarded raw write | Reserved bits are rejected. FDC221x allows the documented reset bit only; FDC211x also allows documented `OUTPUT_GAIN` bits. |
| `0x1E..0x21` | `DRIVE_CURRENT_CHx` | Safe 5-bit helper + explicit raw writer | `setDriveCurrentCode()` writes a documented 5-bit code into the high field. CH2/CH3 are rejected for 2-channel variants. |
| `0x7E` | `MANUFACTURER_ID` | Read-only | Expected `0x5449`. |
| `0x7F` | `DEVICE_ID` | Read-only | Expected `0x3054` for FDC211x and `0x3055` for FDC221x. |

## Startup and conversion policy

The FDC2x1x powers up in Sleep Mode. Public examples configure timing/current/deglitch while asleep, then call `startContinuous(channel)` to select `ACTIVE_CHAN` and clear `SLEEP_MODE_EN`. The library does not claim autoscan conversion unless the user explicitly configures the autoscan/sequence bits.

Low-level register write APIs are for advanced users who have read the datasheet. Safe helpers are recommended for public examples.

## Variant rules

- FDC2112 and FDC2212 are two-channel variants; CH2 and CH3 are rejected.
- FDC2114 and FDC2214 are four-channel variants.
- FDC211x raw output is treated as a 12-bit foundation path in v0.6.x; precise frequency conversion remains `NotSupported`.
- FDC221x raw output is treated as a 28-bit MSB+LSB path.
- Data registers are read-only; reserved/unknown registers are rejected.

## Non-claims

This register map is a code-correctness patch. Hardware validation was not performed by the Developer.


## CONFIG reserved-bit policy

For public writable `CONFIG` values, reserved bits are validated with mask `0x153F` and required pattern `0x1401`. `SENSOR_ACTIVATE_SEL` (bit 11), `REF_CLK_SRC` (bit 9), `INTB_DIS` (bit 7), and `HIGH_CURRENT_DRV` (bit 6) are real options and must not be treated as fixed reserved bits. Safe helpers preserve those options while changing sleep/active-channel state.

For reset modeling, FakeI2C still exposes the documented reset default `CONFIG = 0x2801`; safe writes convert subsequent public writes to the guarded writable pattern.

## MUX_CONFIG variant policy

For two-channel variants (`FDC2112`, `FDC2212`), raw autoscan writes accept `RR_SEQUENCE=b00` and `b11` because both select CH0/CH1. `b01` and `b10` are rejected because they include CH2/CH3. Single-channel helpers write `0x0208 | deglitch` and therefore clear `AUTOSCAN_EN` and `RR_SEQUENCE`.


## HIGH_CURRENT_DRV cross-register policy

`CONFIG.HIGH_CURRENT_DRV` is guarded as CH0 single-channel only. The driver rejects a final state where `HIGH_CURRENT_DRV=1` coexists with `MUX_CONFIG.AUTOSCAN_EN=1`, whether the invalid state would be created through a CONFIG write or a MUX_CONFIG write.

## v0.6.11 hardening

`ERROR_CONFIG` raw writes are now guarded with an allow-list for documented alert/interrupt routing bits. High-level configuration setters require Sleep Mode; raw register access remains explicit advanced access.
