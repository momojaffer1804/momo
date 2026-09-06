#include "TimeManager.h"

// =====================================================
// Constructor
// =====================================================

TimeManager::TimeManager(
    Adafruit_SSD1306* display
) {

    this->display = display;
}


// =====================================================
// Draw Clock / Time Screen
// =====================================================

void TimeManager::drawScreen() {

    struct tm timeInfo;

    if (!getLocalTime(&timeInfo)) {

        display->clearDisplay();

        display->setTextColor(
            SSD1306_WHITE
        );

        display->setTextSize(1);

        display->setCursor(
            42,
            25
        );

        display->println(
            "Loading..."
        );

        display->display();

        return;
    }


    display->clearDisplay();

    display->setTextColor(
        SSD1306_WHITE
    );


    // =================================================
    // Time
    // =================================================

    char timeString[12];

    strftime(
        timeString,
        sizeof(timeString),
        "%I:%M %p",
        &timeInfo
    );


    display->setTextSize(2);


    int timeWidth =
        strlen(timeString) * 12;

    int timeX =
        (128 - timeWidth) / 2;


    display->setCursor(
        timeX,
        4
    );


    display->println(
        timeString
    );


    // =================================================
    // Day
    // =================================================

    char dayString[15];

    strftime(
        dayString,
        sizeof(dayString),
        "%A",
        &timeInfo
    );


    display->setTextSize(1);


    int dayWidth =
        strlen(dayString) * 6;

    int dayX =
        (128 - dayWidth) / 2;


    display->setCursor(
        dayX,
        30
    );


    display->println(
        dayString
    );


    // =================================================
    // Date
    // =================================================

    char dateString[20];

    strftime(
        dateString,
        sizeof(dateString),
        "%d %b %Y",
        &timeInfo
    );


    int dateWidth =
        strlen(dateString) * 6;

    int dateX =
        (128 - dateWidth) / 2;


    display->setCursor(
        dateX,
        45
    );


    display->println(
        dateString
    );


    // =================================================
    // Show Display
    // =================================================

    display->display();
}