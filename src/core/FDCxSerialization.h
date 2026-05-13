#pragma once

#include <stdint.h>
#include <string.h>

inline void fdcxPutU16(uint8_t* output, uint16_t value) {
  output[0] = static_cast<uint8_t>(value & 0xFFu);
  output[1] = static_cast<uint8_t>((value >> 8) & 0xFFu);
}

inline void fdcxPutU32(uint8_t* output, uint32_t value) {
  output[0] = static_cast<uint8_t>(value & 0xFFu);
  output[1] = static_cast<uint8_t>((value >> 8) & 0xFFu);
  output[2] = static_cast<uint8_t>((value >> 16) & 0xFFu);
  output[3] = static_cast<uint8_t>((value >> 24) & 0xFFu);
}

inline uint16_t fdcxGetU16(const uint8_t* input) {
  return static_cast<uint16_t>(input[0]) | static_cast<uint16_t>(static_cast<uint16_t>(input[1]) << 8);
}

inline uint32_t fdcxGetU32(const uint8_t* input) {
  return static_cast<uint32_t>(input[0]) |
         (static_cast<uint32_t>(input[1]) << 8) |
         (static_cast<uint32_t>(input[2]) << 16) |
         (static_cast<uint32_t>(input[3]) << 24);
}

inline void fdcxPutFloat(uint8_t* output, float value) {
  uint32_t encoded = 0;
  memcpy(&encoded, &value, sizeof(float));
  fdcxPutU32(output, encoded);
}

inline float fdcxGetFloat(const uint8_t* input) {
  const uint32_t encoded = fdcxGetU32(input);
  float value = 0.0f;
  memcpy(&value, &encoded, sizeof(float));
  return value;
}
