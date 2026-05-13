#include "FDCxProfileTypes.h"

const char* fdcxProfileStateToString(FDCxProfileState state) {
  switch (state) {
    case FDCxProfileState::Unknown: return "Unknown";
    case FDCxProfileState::Stable: return "Stable";
    case FDCxProfileState::BelowThreshold: return "BelowThreshold";
    case FDCxProfileState::AboveThreshold: return "AboveThreshold";
    case FDCxProfileState::Near: return "Near";
    case FDCxProfileState::Far: return "Far";
    case FDCxProfileState::Empty: return "Empty";
    case FDCxProfileState::Full: return "Full";
    case FDCxProfileState::Partial: return "Partial";
    case FDCxProfileState::Dry: return "Dry";
    case FDCxProfileState::WetCandidate: return "WetCandidate";
    case FDCxProfileState::FrostCandidate: return "FrostCandidate";
    case FDCxProfileState::IceCandidate: return "IceCandidate";
    case FDCxProfileState::MaterialChanged: return "MaterialChanged";
    case FDCxProfileState::Unstable: return "Unstable";
    case FDCxProfileState::Candidate: return "Candidate";
    case FDCxProfileState::Invalid: return "Invalid";
  }
  return "Unknown";
}
