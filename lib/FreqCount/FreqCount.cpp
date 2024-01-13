#include "FreqCount.h"
#include "Arduino.h"

FreqCounter::FreqCounter() {

  tbuffer.clear();
}

void FreqCounter::signal() {
  // Add event to the head
  uint32_t now = millis();
  if (tbuffer.isEmpty()) tbuffer.unshift(now);
  // Add event only if the last one was not too soon. (Expecting less than 300 RPM)
  else if (now - tbuffer.first() > 200) tbuffer.unshift(now);
}

int FreqCounter::getRPM(uint32_t old) {
  if (tbuffer.isEmpty()) return 0;
  
  uint32_t now =  millis();
  uint32_t df = tbuffer.first();
  if (now - df > old) return 0;

  uint32_t dc = 0; uint32_t dl = 0;
  for (int i = 1; i < tbuffer.size(); i++) {
    if (now - tbuffer[i] > old) break;
    dl = tbuffer[i];
    dc = dc + 1;
    if (dc > 3) break; // Calculation from minimum 2, but maximum 3 lapses
  }
  // No rpm if there is just a single measurement (between two events)
  if (dc < 2) return 0;

  uint32_t ll = df - tbuffer[1]; // The last lapse
  float avgMsLapse = 0.0;
  if (now - df < ll) {
    avgMsLapse = (float) (df - dl) / (float) (dc);
  } else {
    avgMsLapse = (float) (now - dl) / (float) (dc + 1); // Assume that an event will happen just now
  }

  float hz = 1000.0 / avgMsLapse;
  float rpm = hz * 60.0; // 1 HZ = 60 RPM

  return (int) rpm;
}

void FreqCounter::reset() {
  tbuffer.clear();
}
