# Temperature Compensation

FDC1004 does not provide a dedicated internal temperature source for this library's compensation path. Temperature must come from the user or an external sensor.

## Linear helper

```cpp
C_comp = C_meas - alpha * (T - T_ref)
```

`alphaPfPerC` is user-supplied and must not be hardcoded as a universal value.

## Lookup helper

`compensateTemperatureLookup()` accepts a sorted table of `{temperatureC, offsetPf}` points and linearly interpolates the offset.

Rules:

- table pointer must not be null
- `count >= 2`
- points must be strictly sorted by temperature
- all values must be finite
- out-of-range temperature returns `InvalidArgument`; it does not silently extrapolate
