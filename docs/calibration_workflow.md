# Calibration Workflow

Recommended flow through Phase 3:

1. Verify basic FDC1004 communication and ID check.
2. Choose CAPDAC-disabled or CAPDAC-enabled mode explicitly.
3. Collect baseline samples with stable environment.
4. Use diagnostics to detect unstable or saturated readings.
5. Optionally use reference-channel and temperature compensation helpers.
6. Feed `FDCxFeatureVector` into experimental profiles.
7. Store calibration profiles externally if needed; the library does not require EEPROM/NVS/Flash.
