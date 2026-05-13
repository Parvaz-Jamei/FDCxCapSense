# Hardware Validation Matrix

Hardware validation is owner-side and was not performed by the Developer.

| Date | Owner | Board | Arduino core version | Sensor chip | I2C address | Pull-up resistor | Sensor board/module | Electrode/resonator notes | Example | Compile status | Hardware status | Serial evidence | Notes |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
| TBD | Owner | Arduino UNO | TBD | FDC1004 | `0x50` | TBD | TBD | TBD | `FDC1004_BasicRead` | NOT RUN | NOT PERFORMED | Not recorded | Owner-side smoke test required |
| TBD | Owner | ESP32 | TBD | FDC1004 | `0x50` | TBD | TBD | TBD | `FDC1004_BasicRead` | NOT RUN | NOT PERFORMED | Not recorded | Owner-side Wire/Wire1 test recommended |
| TBD | Owner | TBD | TBD | FDC2214 | `0x2A/0x2B` | TBD | TBD | LC tank/resonator TBD | `FDC2x1x_BasicFrequency` | NOT RUN | NOT PERFORMED | Not recorded | Owner-side resonator setup required |

## Evidence policy

Do not mark hardware validation as PASS unless the owner records:

- board name
- Arduino core version
- sensor chip and module
- wiring and I2C address
- pull-up resistor information
- electrode or resonator notes
- example sketch used
- Serial Monitor output or log
- date and tester

Developer-created ZIPs must keep hardware validation as `NOT PERFORMED` unless owner evidence is supplied.
