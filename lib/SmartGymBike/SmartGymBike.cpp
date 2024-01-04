#include "SmartGymBike.h"
#include "BikeStat.h"

constexpr int SmartGymBike::presetLevels[PRESETLEVELS] = { 100, 1000, 1500, 2000, 2500, 3000 };
volatile bool SmartGymBike::pedalPush = false;

SmartGymBike::SmartGymBike() {

  // Setup resistance controller
  pinMode(PIN_MP, OUTPUT);
  pinMode(PIN_MN, OUTPUT);
  digitalWrite(PIN_MP, LOW);
  digitalWrite(PIN_MN, LOW);

  targetResistance = analogRead(PIN_POS);
  targetReached = true;

  cadenceMeter.reset();
  lastLoop1 = 0;
  lastLoop2 = 0;

  //attachInterrupt(PIN_SPEED, cadenceInt, FALLING);
}
  
int SmartGymBike::readResistance() {
  int res = analogRead(PIN_POS);
  BikeStat& bikeStat = BikeStat::getInstance();
  bikeStat.bikeResistance = res;
  return res;
}

int SmartGymBike::readCadence() {
  int cad = cadenceMeter.getRPM();
  BikeStat& bikeStat = BikeStat::getInstance();
  bikeStat.bikeCadence = cad;
  return cad;
}

void SmartGymBike::resistanceStop() {
  digitalWrite(PIN_MP, LOW);
  digitalWrite(PIN_MN, LOW);
  targetResistance = analogRead(PIN_POS);
  setResistance(targetResistance);
  targetReached = true;
  
  //readResistance(); // readLevel reads resistance as well
  readLevel();
}

void SmartGymBike::resistanceUp(int step) {

  setResistance(targetResistance + step);
}

void SmartGymBike::resistanceDown(int step) {

  setResistance(targetResistance - step);
}

void SmartGymBike::setResistance(int res) {

  targetResistance = res;
  if (targetResistance < RES_MIN) targetResistance = RES_MIN;
  if (targetResistance > RES_MAX) targetResistance = RES_MAX;
  targetReached = false;
  Serial.print("Set resistance to: ");
  Serial.println(targetResistance);
}

void SmartGymBike::loop() {

  BikeStat& bikeStat = BikeStat::getInstance();

  // Signal push for cadence
  if (pedalPush) {
    pedalPush = false;
    cadenceMeter.signal();
    bikeStat.bikeRevs += 1;
  }

  if (millis() - lastLoop1 > 5) {
  
    if (!targetReached) {
      int mpos = readResistance();
    
    // Set resistace to target
      if (targetResistance - mpos > 30) {
        digitalWrite(PIN_MP, HIGH);
        digitalWrite(PIN_MN, LOW);
        Serial.print(targetResistance);
        Serial.print(":");
        Serial.print(mpos);
        Serial.println("+");      
      } 
      else if (mpos - targetResistance > 30) {
        digitalWrite(PIN_MN, HIGH);
        digitalWrite(PIN_MP, LOW);
        Serial.print(targetResistance);
        Serial.print(":");
        Serial.print(mpos);
        Serial.println("-");
      }
      else {
        digitalWrite(PIN_MP, LOW);
        digitalWrite(PIN_MN, LOW);
        Serial.print(targetResistance);
        Serial.print(":");
        Serial.print(mpos);
        Serial.println("=");
        targetReached = true;

        readLevel();
      }
    }
    lastLoop1 = millis();
  }

  if (millis() - lastLoop2 > 200) {
    int cad = readCadence();
    int lvl = readLevel();
    int res = readResistance();

    if (cad > 0) {
      bikeStat.bikeTime += (millis() - lastLoop2);
    }
    lastLoop2 = millis();
  }

}

void SmartGymBike::setLevel(uint8_t level) {
  if (level > PRESETLEVELS-1) level = PRESETLEVELS-1;
  setResistance(presetLevels[level]);
}

uint8_t SmartGymBike::readLevel() {
  int lvl = whichLevel(readResistance());
  BikeStat& bikeStat = BikeStat::getInstance();
  bikeStat.bikeLevel = lvl;
  return lvl;
}

uint8_t SmartGymBike::whichLevel(int r) {
  int mi = 0;
  
  for (int i=0; i<PRESETLEVELS; i++) {
    if (abs(r - presetLevels[i]) < abs(r-presetLevels[mi])) {
      mi = i;
    }
  }
  return mi;
}

int SmartGymBike::estimatePower(int cadence, int resistance) {
  BikeStat& bikeStat = BikeStat::getInstance();
  // Estimation function:
  // power = 41.679*lev + -1.375*lev*cad + 0.01338*lev*cad*cad + 1.855*cad + -77.531
  float lev = (float) bikeStat.bikeLevel;
  float cad = (float) bikeStat.bikeCadence;
  if (cad < 40) return 0;
  float power = 41.679*lev + -1.375*lev*cad + 0.01338*lev*cad*cad + 1.855*cad + -77.531;
  if (power < 0) power = 0;
  return int(power*5)/5;
}
