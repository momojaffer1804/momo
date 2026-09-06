#ifndef POMODORO_H
#define POMODORO_H

#include <Arduino.h>

enum PomodoroState {
    POMODORO_FOCUS,
    POMODORO_BREAK
};

class Pomodoro {

public:

    Pomodoro();

    void startPause();
    void reset();
    void update();

    // Set custom Focus and Break times in minutes
    void setTimer(unsigned long focusMinutes,
                  unsigned long breakMinutes);

    bool isRunning();

    PomodoroState getState();

    unsigned long getRemainingSeconds();

private:

    bool running;

    PomodoroState state;

    unsigned long remainingSeconds;
    unsigned long lastUpdate;

    // Currently selected timer durations
    unsigned long focusSeconds;
    unsigned long breakSeconds;

    void switchPhase();
};

#endif