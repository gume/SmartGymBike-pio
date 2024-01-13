#include "BikeDisplay.h"

#include <WiFi.h>


int BikeDisplay::lastRoadRevs;

BikeDisplay::BikeDisplay() {
}

void BikeDisplay::setup() {
  
  if(!display->begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("ERROR! SSD1306 allocation failed"));
    // Do something ???
  }
  display->clearDisplay();
  display->setRotation(1);

  display->setTextSize(1);      // Normal 1:1 pixel scale
  display->setTextColor(SSD1306_WHITE); // Draw white text
  display->setTextWrap(false);
  
  refreshInterval = 500;  // 2 FPS
  event = false;
  screenNum = 0;
}


void BikeDisplay::loop() {

  uint32_t now = millis();
  // Check display event
  if (event) {
    if (now - eventTime < eventHoldTime) {
      // Do nothing 
      return;
    } else {
      event = false;
    }
  }

  // Check refresh
  if (now - lastRefresh < refreshInterval) {
    // Do nothing
    return;
  }

  screenDisplay[screenNum]();  
  display->display();
  
  lastRefresh = now;
}

void BikeDisplay::toast(String message, uint32_t duration) {
  // Display toast immediately
  eventTime = millis();
  eventHoldTime = duration;
  event = true;
  
  display->clearDisplay();
  display->setCursor(0, 0);     // Start at top-left corner
  display->write(message.c_str());
  display->display();
}

void BikeDisplay::showBikeTime(int ypos) {

  BikeStat& bikeStat = BikeStat::getInstance();

  // Display bikeTime
  int h = bikeStat.bikeTime / 1000 / 60 / 60;
  int m = (bikeStat.bikeTime / 1000 / 60) % 60;
  int s = (bikeStat.bikeTime / 1000) % 60;
  if (h > 0) {
    display->setCursor(0, ypos);
    display->write(String(h).c_str());
    display->setCursor(4,ypos);
    display->write(":");
  }
  String ms = "0";
  if (m > 9) ms = String(m);
  else ms = ms + String(m);
  display->setCursor(8,ypos);
  display->write(ms.c_str());
  display->setCursor(17,ypos);
  display->write(":");
  String ss = "0";
  if (s > 9) ss = String(s);
  else ss = ss + String(s);
  display->setCursor(21,ypos);
  display->write(ss.c_str());
  display->drawLine(0, 9+ypos, 31, 9+ypos, SSD1306_WHITE);
}

void BikeDisplay::statScreen0() {

  BikeStat& bikeStat = BikeStat::getInstance();

  display->setRotation(1);
  display->clearDisplay();
  showBikeTime(0);
  
  // Display Level
  writeCenter("Level", 21);
  writeCenter(String(bikeStat.bikeLevel), 30);

  // Display Power
  writeCenter("Power", 48);
  writeCenter(String(bikeStat.bikePower), 57);

  // Display Cadence
  writeCenter("RPM", 75);
  writeCenter(String(bikeStat.bikeCadence), 84);

  // Display Revs
  writeCenter("Revs", 102);
  writeCenter(String(bikeStat.bikeRevs), 111);
}

void BikeDisplay::statScreen1() {

  BikeStat& bikeStat = BikeStat::getInstance();

  display->setRotation(1);
  display->clearDisplay();
  showBikeTime(0);

  // Three bars
  drawProgressbarH(1, 12, 10, 85, bikeStat.bikeResistance/32);
  drawProgressbarH(11, 12, 10, 85, bikeStat.bikeCadence-20);
  drawProgressbarH(21, 12, 10, 85, bikeStat.bikePower/3);
  writeCenter(String(bikeStat.bikeRevs), 105);
}

void BikeDisplay::statScreen2() {

  BikeStat& bikeStat = BikeStat::getInstance();

  int m = (bikeStat.bikeTime / 1000 / 60) % 60;
  int s = (bikeStat.bikeTime / 1000) % 60;

  String ms = "0";
  if (m > 9) ms = String(m);
  else ms = ms + String(m);
  String ss = "0";
  if (s > 9) ss = String(s);
  else ss = ss + String(s);

  display->setRotation(1);
  display->clearDisplay();
  display->setTextSize(3);
  display->setCursor(0, 40);
  display->write(ms.c_str());  
  display->setCursor(0, 70);
  display->write(ss.c_str());
  display->setTextSize(1);
}

void BikeDisplay::roadScreen() {
  BikeStat& bikeStat = BikeStat::getInstance();
  int x = bikeStat.bikeRevs;
  if (x == lastRoadRevs) return;

  display->setRotation(1);
  display->clearDisplay();
  display->drawFastVLine(0, 0, 110, SSD1306_WHITE);
  display->drawFastVLine(31, 0, 110, SSD1306_WHITE);

  if ((x/50) * 50 == x) {
    // Display 3 values
    writeCenter(String(x), 100);
    writeCenter(String(x+50), 50);
    writeCenter(String(x+100), 0);
  } else {
    // Display 2 values only
    int x1 = ((x + 49) / 50) * 50; // The next round 50's
    int y1 = x % 50;
    writeCenter(String(x1), 50 + y1);
    writeCenter(String(x1 + 50), y1);
  }

  lastRoadRevs = x;
}


void BikeDisplay::levelSetScreen() {

  BikeStat& bikeStat = BikeStat::getInstance();

  display->setRotation(1);
  display->clearDisplay();
  display->setTextSize(3);
  display->setCursor(0,30);
  display->write(String("L"+String(bikeStat.bikeLevel)).c_str());
  display->setTextSize(1);
  writeCenter(String(bikeStat.bikeResistance), 70);
}

void BikeDisplay::aboutScreen() {

  BikeStat& bikeStat = BikeStat::getInstance();

  display->setRotation(2);
  display->clearDisplay();
  display->setTextSize(1);
  display->setCursor(0,0);
  display->print("SW");
  display->setCursor(28,0);
  display->print(bikeStat.bikeRikeVersion);
  display->setCursor(0,10);
  display->print("IP");
  display->setCursor(28,10);
  display->print(WiFi.localIP().toString());
  display->setCursor(0,20);
  display->print("MQTT");
  display->setCursor(28,20);
  display->print(bikeStat.mqttBroker);
}

void BikeDisplay::screenChange(int newNum) {
  screenNum = newNum;
  if (screenNum >= SCREENS) screenNum = 0;
  if (screenNum < 0) screenNum = SCREENS - 1;

  if (screenNum == 3) lastRoadRevs = -1;
}

void BikeDisplay::screenNext(bool back) {
  if (back) screenChange(screenNum - 1);
  else screenChange(screenNum + 1);
}

void BikeDisplay::drawProgressbarH(int x,int y, int width, int height, int progress) {
   progress = progress > 100 ? 100 : progress; // set the progress value to 100
   progress = progress < 0 ? 0 :progress; // start the counting to 0-100
   int bar = ((height-4) * progress) / 100;
   display->drawRect(x, y, width, height, SSD1306_WHITE);
   display->fillRect(x+2, y+height-2-bar, width-4, bar, SSD1306_WHITE);
}

void BikeDisplay::writeCenter(String text, int ypos) {
  display->setCursor(16-(text.length()*5/2),ypos);
  display->print(text.c_str());
}
