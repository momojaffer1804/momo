#ifndef POMODORO_MANAGER_H
#define POMODORO_MANAGER_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include "Pomodoro.h"

class PomodoroManager {
public:

    PomodoroManager(
        Adafruit_SSD1306* display,
        Pomodoro* pomodoro
    );

    void drawScreen();

private:

    Adafruit_SSD1306* display;
    Pomodoro* pomodoro;
};

#endif