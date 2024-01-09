#ifndef BikeDisplay_h
#define BikeDisplay_h

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "BikeStat.h"

typedef void (*screenCallBack)();

class BikeDisplay {
    public:
    BikeDisplay();

    void setup();
    void loop();
    void toast(String message, uint32_t duration);
    void screenChange(int newNum);
    void screenNext(bool back = false);

    static Adafruit_SSD1306 *display;

    private:
    static void statScreen0();
    static void statScreen1();
    static void statScreen2();
    static void roadScreen();
    static void levelSetScreen();
    static void aboutScreen();
 
    static void showBikeTime(int ypos);
    static void drawProgressbarH(int x,int y, int width, int height, int progress);
    static void writeCenter(String text, int ypos);    

    void levelChange(int bp);

    bool event;
    uint32_t eventTime;  // When a dipsplay event happened
    uint32_t eventHoldTime; // How long display will stay

    uint32_t lastRefresh; // Last time display was refreshed
    uint32_t refreshInterval; // refresh period time

    #define SCREENS 6
    int screenNum;  // Display various screens
    screenCallBack screenDisplay[SCREENS] = { &statScreen0, &statScreen1, &statScreen2, &roadScreen, &levelSetScreen, &aboutScreen};

    static int lastRoadRevs;

};

#endif