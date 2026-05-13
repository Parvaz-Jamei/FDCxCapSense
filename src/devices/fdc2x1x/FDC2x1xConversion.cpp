#include "FDC2x1xConversion.h"
#include "../../core/FDCxMath.h"

bool fdc2x1xIsFiniteFloat(float value) {
  return fdcxIsFiniteFloat(value);
}

FDCxStatus fdc2x1xDeriveCapacitancePf(const FDC2x1xFrequency& frequency,
                                      const FDC2x1xLCParams& params,
                                      FDC2x1xDerivedCapacitance& output) {
  output = FDC2x1xDerivedCapacitance();
  output.variant = frequency.variant;
  output.channel = frequency.channel;
  if (frequency.status != FDCxStatus::Ok) {
    output.status = frequency.status;
    return output.status;
  }
  if (!fdc2x1xIsFiniteFloat(frequency.frequencyHz) || frequency.frequencyHz <= 0.0f ||
      !fdc2x1xIsFiniteFloat(params.inductanceUH) || params.inductanceUH <= 0.0f ||
      !fdc2x1xIsFiniteFloat(params.parasiticCapacitancePf) || params.parasiticCapacitancePf < 0.0f) {
    output.status = FDCxStatus::InvalidArgument;
    return output.status;
  }
  // Independent LC formula: f = 1/(2*pi*sqrt(L*C)). The capacitance result is model-dependent.
  // params.traceReferenceClockHz is optional trace metadata and is not used here because
  // frequency.frequencyHz is already the converted resonant frequency.
  const float pi = 3.14159265358979323846f;
  const float lHenries = params.inductanceUH * 1.0e-6f;
  const float omega = 2.0f * pi * frequency.frequencyHz;
  const float totalFarads = 1.0f / (omega * omega * lHenries);
  const float totalPf = totalFarads * 1.0e12f;
  const float derivedPf = totalPf - params.parasiticCapacitancePf;
  if (!fdc2x1xIsFiniteFloat(derivedPf) || derivedPf < 0.0f) {
    output.status = FDCxStatus::InvalidArgument;
    output.modelWarning = "Invalid LC parameters produced negative derived capacitance";
    return output.status;
  }
  output.capacitancePf = derivedPf;
  output.isDerived = true;
  output.status = FDCxStatus::Ok;
  output.modelWarning = "Derived capacitance from FDC2x1x resonant frequency and user-supplied LC parameters";
  return output.status;
}
