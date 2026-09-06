#include "PomodoroManager.h"

// =====================================================
// Constructor
// =====================================================

PomodoroManager::PomodoroManager(
    Adafruit_SSD1306* display,
    Pomodoro* pomodoro
) {

    this->display = display;
    this->pomodoro = pomodoro;
}

// =====================================================
// Draw Pomodoro Screen
// =====================================================

void PomodoroManager::drawScreen() {

    display->clearDisplay();

    display->setTextColor(
        SSD1306_WHITE
    );

    display->setTextSize(1);

    // =================================================
    // Title
    // =================================================

    const char* title;

    if (
        pomodoro->getState() ==
        POMODORO_FOCUS
    ) {

        title = "FOCUS";

    } else {

        title = "BREAK";
    }

    int titleWidth =
        strlen(title) * 6;

    display->setCursor(
        (128 - titleWidth) / 2,
        0
    );

    display->println(
        title
    );

    // =================================================
    // Remaining Time
    // =================================================

    unsigned long totalSeconds =
        pomodoro->getRemainingSeconds();

    unsigned long minutes =
        totalSeconds / 60;

    unsigned long seconds =
        totalSeconds % 60;

    char timerString[8];

    snprintf(
        timerString,
        sizeof(timerString),
        "%02lu:%02lu",
        minutes,
        seconds
    );

    display->setTextSize(3);

    int timerWidth =
        strlen(timerString) * 18;

    int timerX =
        (128 - timerWidth) / 2;

    display->setCursor(
        timerX,
        18
    );

    display->println(
        timerString
    );

    // =================================================
    // Status
    // =================================================

    display->setTextSize(1);

    const char* status;

    if (
        pomodoro->isRunning()
    ) {

        status = "RUNNING";

    } else {

        status = "PAUSED";
    }

    int statusWidth =
        strlen(status) * 6;

    display->setCursor(
        (128 - statusWidth) / 2,
        50
    );

    display->println(
        status
    );

    // =================================================
    // Show Display
    // =================================================

    display->display();
}