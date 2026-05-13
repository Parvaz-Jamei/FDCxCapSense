#include <AUnit.h>
#include <FDCxCapSense.h>

using namespace aunit;

test(surface_returns_candidate_not_final_decision) {
  FDCxSurfaceStateConfig config;
  FDCxSurfaceStateProfile profile(config);
  FDCxProfileResult result = profile.update(2.0f, -1.0f, 0.01f, 0.0f, 0);
  assertEqual(static_cast<int>(FDCxProfileState::IceCandidate), static_cast<int>(result.state));
  assertTrue(result.confidence >= 0.0f && result.confidence <= 1.0f);
}

void setup() {
  Serial.begin(115200);
  TestRunner::run();
}

void loop() {}
