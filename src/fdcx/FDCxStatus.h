#pragma once

#include <stdint.h>

enum class FDCxStatus : uint8_t {
  Ok = 0,
  I2cError,
  Timeout,
  InvalidArgument,
  DeviceNotFound,
  InvalidDeviceId,
  MeasurementNotReady,
  NotSupported
};

inline const char* fdcxStatusToString(FDCxStatus status) {
  switch (status) {
    case FDCxStatus::Ok:
      return "Ok";
    case FDCxStatus::I2cError:
      return "I2cError";
    case FDCxStatus::Timeout:
      return "Timeout";
    case FDCxStatus::InvalidArgument:
      return "InvalidArgument";
    case FDCxStatus::DeviceNotFound:
      return "DeviceNotFound";
    case FDCxStatus::InvalidDeviceId:
      return "InvalidDeviceId";
    case FDCxStatus::MeasurementNotReady:
      return "MeasurementNotReady";
    case FDCxStatus::NotSupported:
      return "NotSupported";
  }
  return "Unknown";
}
