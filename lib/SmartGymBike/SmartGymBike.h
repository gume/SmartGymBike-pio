#pragma once

#include "FreqCount.h"
#include "Arduino.h"


#define PIN_MP 26
#define PIN_MN 27

#define PIN_POS 33
#define PIN_SPEED 32

#define PRESETLEVELS 6

class SmartGymBike {
public:
  SmartGymBike();

  int readCadence();

  // Resistance with fine resolution
  void resistanceUp(int step = 100);
  void resistanceDown(int step = 100);
  void setResistance(int);
  void resistanceStop();
  
  int readResistance();

  // Level is a corse resolution for resistance
  void setLevel(uint8_t);
  uint8_t readLevel();
  uint8_t whichLevel(int);

  int estimatePower(int cadence, int resistence);
    
  void loop();

  const static int RES_MAX = 3200;  // Very instable at 4000
  const static int RES_MIN = 100;

  volatile static bool pedalPush;
  
private:
  int targetResistance;  
  bool targetReached;

  FreqCounter cadenceMeter;
  uint32_t lastLoop1;
  uint32_t lastLoop2;

  static const int presetLevels[PRESETLEVELS];
};
