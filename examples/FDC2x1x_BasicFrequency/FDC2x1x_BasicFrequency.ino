#include <FDCxCapSense_FDC2x1x.h>
#include <Wire.h>

FDC2x1x fdc;

void setup() {
  Serial.begin(115200);
  const unsigned long serialWaitStart = millis();
  while (!Serial && (millis() - serialWaitStart < 1500UL)) { delay(10); }
  Wire.begin();

  if (fdc.begin(Wire, FDC2x1xVariant::FDC2214) != FDCxStatus::Ok) {
    Serial.println("FDC2x1x init failed or identity check failed");
    return;
  }

  // These are example configuration values. Real hardware values must be selected
  // from the datasheet based on the LC tank, reference clock and sensor topology.
  FDCxStatus st = fdc.setReferenceClockSource(FDC2x1xReferenceClockSource::ExternalClockInput);
  if (st == FDCxStatus::Ok) {
    st = fdc.configureSingleChannel(FDC2x1xChannel::CH0,
                                    0x0100,
                                    0x0100,
                                    FDC2x1xSensorTopology::SingleEnded,
                                    1,
                                    15,
                                    FDC2x1xDeglitch::MHz_10);
  }
  if (st != FDCxStatus::Ok) {
    Serial.println("FDC2x1x configuration failed");
    return;
  }

  FDC2x1xFrequency reading;
  if (fdc.waitDataReady(FDC2x1xChannel::CH0, 100) != FDCxStatus::Ok) {
    Serial.println("FDC2x1x CH0 data not ready");
    delay(250);
    return;
  }
  if (fdc.readFrequencyHzFromRegisters(FDC2x1xChannel::CH0, 40000000.0f, reading) == FDCxStatus::Ok) {
    Serial.print("Frequency Hz: ");
    Serial.println(reading.frequencyHz, 2);
  }
}

void loop() {}
