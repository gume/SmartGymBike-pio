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
  void resistanceLevelUp();
  void resistanceDown(int step = 100);
  void resistanceLevelDown();
  void resistanceSet(int);
  void resistanceStop();
  
  int readResistance();

  // Level is a corse resolution for resistance
  void setLevel(uint8_t);
  int readLevel();
  int whichLevel(int);

  int estimatePower(int cadence, int resistence);
    
  void loop();

  const static int RES_MAX = 3200;  
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
