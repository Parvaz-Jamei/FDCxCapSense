# FDC1004 / FDC1004-Q1 Notes

These notes are planning notes only. They are not a driver implementation.

## High-level device facts to use for Phase 1 planning

- FDC1004 is the first implementation target.
- FDC1004-Q1 is the automotive-qualified variant and should be documented separately from the non-Q1 part.
- Device family focus: capacitive sensing using grounded capacitor sensors.
- Channels: 4 input channels.
- Interface: I2C.
- Input range: ±15 pF.
- Offset compensation: up to about 100 pF.
- Programmable output rates: 100, 200, and 400 samples/s.
- Active shield drivers are important for EMI/parasitic effects and sensing-field control.
- Future code must avoid infinite blocking loops and return clear error codes.

## Phase 1 implementation notes

The Phase 1 driver should be written from official TI documentation using clean-room code. The expected design direction:

- `begin(TwoWire* wire = &Wire, ...)`
- register read/write helpers
- timeout-aware I2C operations
- typed channel selection
- sample-rate configuration
- raw measurement read path
- CAPDAC configuration support only as needed for a basic driver
- fake I2C/register simulator before hardware test claims

## Not in Phase 1

- Auto-CAPDAC algorithm beyond basic register-level support
- Drift compensation
- Temperature compensation
- Application profiles
- Ice / wet / frost / material / liquid classification
- Automotive claim logic


## v0.6.8 API notes

- High-level FDC1004 measurement helpers are single-ended only. Differential measurement configuration is not exposed as a high-level API in this package.
- `readCapacitancePf()` now requires the selected measurement slot to have been configured and reads the latest completed measurement registers; `readLastCapacitancePf()` is an explicit alias. `readSingleMeasurement()` remains the recommended safe path when a fresh single-shot conversion is required.
- `OFFSET_CAL_CINx` and `GAIN_CAL_CINx` are low-level hardware calibration registers and are not part of the high-level calibration API yet.
- FDC config cache updates occur only after successful writes, so I2C write failures do not silently diverge the cache from the device.


## v0.6.11 latest-register read note

`readCapacitancePf()` and `readLastCapacitancePf()` read the latest completed measurement registers. For a fresh single-shot conversion, call `readSingleMeasurement()`.
