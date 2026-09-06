#ifndef CALENDAR_MANAGER_H
#define CALENDAR_MANAGER_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>

class CalendarManager {
public:
    CalendarManager(
        Adafruit_SSD1306* display
    );

    void drawScreen();

private:
    Adafruit_SSD1306* display;
};

#endif