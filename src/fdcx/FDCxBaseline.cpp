#include "FDCxBaseline.h"
#include <float.h>

FDCxBaselineTracker::FDCxBaselineTracker(const FDCxBaselineConfig& config) : _config(config) {
  if (_config.warmupSamples == 0) {
    _config.warmupSamples = 1;
  }
  if (_config.minStableSamples == 0) {
    _config.minStableSamples = 1;
  }
  if (!isFinite(_config.maxNoisePf) || _config.maxNoisePf < 0.0f) {
    _config.maxNoisePf = 0.0f;
  }
  _config.learningRate = clampLearningRate(_config.learningRate);
  if (!isFinite(_config.driftLimitPf) || _config.driftLimitPf < 0.0f) {
    _config.driftLimitPf = 0.0f;
  }
  reset();
}

void FDCxBaselineTracker::reset() {
  _state = FDCxBaselineState();
  _stableSamples = 0;
}

bool FDCxBaselineTracker::update(float capacitancePf) {
  if (!isFinite(capacitancePf)) {
    return false;
  }

  if (_state.sampleCount < 0xFFFFFFFFu) {
    ++_state.sampleCount;
  }

  if (_state.sampleCount == 1u) {
    _state.baselinePf = capacitancePf;
    _state.noisePf = 0.0f;
    _state.driftPf = 0.0f;
    return false;
  }

  const float delta = capacitancePf - _state.baselinePf;
  const float absDelta = absFloat(delta);
  _state.driftPf = delta;
  _state.noisePf = (_state.noisePf * 0.8f) + (absDelta * 0.2f);

  if (!_state.learned) {
    const float adaptiveRate = 1.0f / static_cast<float>(_state.sampleCount);
    _state.baselinePf += delta * adaptiveRate;

    if (_state.sampleCount >= _config.warmupSamples && absDelta <= _config.maxNoisePf) {
      if (_stableSamples < 0xFFFFu) {
        ++_stableSamples;
      }
    } else {
      _stableSamples = 0;
    }

    if (_stableSamples >= _config.minStableSamples) {
      _state.learned = true;
      return true;
    }
    return false;
  }

  if (absDelta > _config.driftLimitPf || absDelta > _config.maxNoisePf) {
    return false;
  }

  _state.baselinePf += delta * _config.learningRate;
  return true;
}

FDCxBaselineState FDCxBaselineTracker::state() const {
  return _state;
}

float FDCxBaselineTracker::deltaFromBaseline(float capacitancePf) const {
  if (!isFinite(capacitancePf)) {
    return 0.0f;
  }
  return capacitancePf - _state.baselinePf;
}

bool FDCxBaselineTracker::isFinite(float value) {
  return value == value && value <= FLT_MAX && value >= -FLT_MAX;
}

float FDCxBaselineTracker::absFloat(float value) {
  return value < 0.0f ? -value : value;
}

float FDCxBaselineTracker::clampLearningRate(float value) {
  if (!isFinite(value)) {
    return 0.05f;
  }
  if (value < 0.0f) {
    return 0.0f;
  }
  if (value > 1.0f) {
    return 1.0f;
  }
  return value;
}
