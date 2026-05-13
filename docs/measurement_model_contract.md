# Measurement Model Contract

FDC1004 samples are direct capacitance samples. FDC2x1x samples are resonant/frequency samples; any capacitance value from FDC2x1x is derived from an LC model and must carry derived-capacitance metadata. Application profiles must consume `FDCxSample` or `FDCxFeatureVector` with family/model metadata.

Raw float inputs are compatibility helpers only, not the official profile API. They can be hidden from builds with:

```cpp
#define FDCX_DISABLE_LEGACY_PROFILE_HELPERS
```

## Required metadata for current experimental profiles

Current Phase 3 profiles support only this sample type:

- `family = FDCxFamily::FDC1004_DirectCDC`
- `model = FDCxMeasurementModel::DirectCapacitance`
- `hasCapacitance = true`
- `capacitanceIsDirect = true`
- `capacitanceIsDerived = false`
- `sample.status = FDCxStatus::Ok`

Any other input is rejected. This includes unknown family/model, missing capacitance, non-Ok samples, FDC2x1x resonant samples, and derived capacitance samples.

## FDC2x1x rule

FDC2x1x support is explicit, frequency-first, and separate. `readFrequencyHz()` is the primary path for FDC221x. Derived capacitance must be reported as model-dependent and must not be treated as equivalent to FDC1004 direct capacitance unless a future profile explicitly declares support and documents the conversion assumptions.
