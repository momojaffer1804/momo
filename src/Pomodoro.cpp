#include "Pomodoro.h"


// =====================================================
// Constructor
// =====================================================

Pomodoro::Pomodoro() {

    running = false;

    state = POMODORO_FOCUS;

    // Default timer
    // 50 minutes Focus
    // 10 minutes Break

    focusSeconds = 50 * 60;
    breakSeconds = 10 * 60;

    remainingSeconds = focusSeconds;

    lastUpdate = millis();
}


// =====================================================
// Start / Pause
// =====================================================

void Pomodoro::startPause() {

    running = !running;

    lastUpdate = millis();
}


// =====================================================
// Reset
// =====================================================

void Pomodoro::reset() {

    running = false;

    // Always return to Focus
    state = POMODORO_FOCUS;

    // IMPORTANT:
    // Reset uses the currently selected Focus time

    remainingSeconds = focusSeconds;

    lastUpdate = millis();
}


// =====================================================
// Set Custom Timer
// =====================================================

void Pomodoro::setTimer(unsigned long focusMinutes,
                         unsigned long breakMinutes) {

    // Convert minutes to seconds

    focusSeconds = focusMinutes * 60;

    breakSeconds = breakMinutes * 60;

    // Stop current timer

    running = false;

    // Start from Focus

    state = POMODORO_FOCUS;

    // Show selected Focus time

    remainingSeconds = focusSeconds;

    lastUpdate = millis();
}


// =====================================================
// Update Timer
// =====================================================

void Pomodoro::update() {

    if (!running)
        return;

    unsigned long now = millis();

    if (now - lastUpdate >= 1000) {

        unsigned long elapsed =
            (now - lastUpdate) / 1000;

        lastUpdate += elapsed * 1000;

        if (elapsed >= remainingSeconds) {

            remainingSeconds = 0;

            switchPhase();

        }
        else {

            remainingSeconds -= elapsed;
        }
    }
}


// =====================================================
// Switch Focus / Break
// =====================================================

void Pomodoro::switchPhase() {

    if (state == POMODORO_FOCUS) {

        // Focus finished
        // Start Break

        state = POMODORO_BREAK;

        remainingSeconds = breakSeconds;

    }
    else {

        // Break finished
        // Start Focus

        state = POMODORO_FOCUS;

        remainingSeconds = focusSeconds;
    }
}


// =====================================================
// Status
// =====================================================

bool Pomodoro::isRunning() {

    return running;
}


PomodoroState Pomodoro::getState() {

    return state;
}


unsigned long Pomodoro::getRemainingSeconds() {

    return remainingSeconds;
}