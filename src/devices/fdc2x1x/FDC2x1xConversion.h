#pragma once
#include "FDC2x1xFrequency.h"
#include "FDC2x1xDerivedCapacitance.h"
#include "FDC2x1xResonator.h"

bool fdc2x1xIsFiniteFloat(float value);
FDCxStatus fdc2x1xDeriveCapacitancePf(const FDC2x1xFrequency& frequency,
                                      const FDC2x1xLCParams& params,
                                      FDC2x1xDerivedCapacitance& output);
