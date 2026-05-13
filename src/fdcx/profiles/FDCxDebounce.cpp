#include "FDCxDebounce.h"

void FDCxDebounce::reset() {
  _count = 0;
}

bool FDCxDebounce::update(bool active, uint8_t requiredSamples) {
  if (requiredSamples == 0u) {
    requiredSamples = 1u;
  }
  if (!active) {
    _count = 0;
    return false;
  }
  if (_count < requiredSamples) {
    ++_count;
  }
  return _count >= requiredSamples;
}

uint8_t FDCxDebounce::count() const {
  return _count;
}
