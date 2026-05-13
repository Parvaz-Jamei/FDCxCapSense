# I2C and Multi-Device Notes

FDC1004 uses a 7-bit I2C address of `0x50` by default. The driver accepts an address argument in `begin()` for testability and board integration.

Measurement polling is timeout-bounded. Low-level Wire bus hang behavior may depend on the Arduino core. Hardware validation should check I2C recovery and timeout behavior on the target board/core.

No hardware bus-recovery behavior is claimed by this package.
