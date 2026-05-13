#pragma once

#include <stdint.h>

class FDCxDebounce {
 public:
  FDCxDebounce() = default;

  void reset();
  bool update(bool active, uint8_t requiredSamples);
  uint8_t count() const;

 private:
  uint8_t _count = 0;
};
