#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <time.h>

class TimeManager {

public:

    TimeManager(
        Adafruit_SSD1306* display
    );

    void drawScreen();

private:

    Adafruit_SSD1306* display;
};

#endif