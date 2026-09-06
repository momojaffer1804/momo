#include "CalendarManager.h"

// =====================================================
// Constructor
// =====================================================

CalendarManager::CalendarManager(
    Adafruit_SSD1306* display
) {
    this->display = display;
}

// =====================================================
// Draw Calendar Screen
// =====================================================

void CalendarManager::drawScreen() {

    struct tm timeInfo;

    if (!getLocalTime(&timeInfo)) {

        display->clearDisplay();

        display->setTextColor(
            SSD1306_WHITE
        );

        display->setTextSize(1);

        display->setCursor(
            38,
            25
        );

        display->println(
            "Loading..."
        );

        display->display();

        return;
    }

    int year =
        timeInfo.tm_year + 1900;

    int month =
        timeInfo.tm_mon;

    int today =
        timeInfo.tm_mday;

    const int daysInMonth[] = {

        31,
        28,
        31,
        30,
        31,
        30,
        31,
        31,
        30,
        31,
        30,
        31
    };

    int totalDays =
        daysInMonth[month];

    // =================================================
    // Leap Year
    // =================================================

    if (
        month == 1 &&
        (
            (year % 400 == 0) ||
            (
                year % 4 == 0 &&
                year % 100 != 0
            )
        )
    ) {

        totalDays = 29;
    }

    // =================================================
    // Find First Day Of Month
    // =================================================

    struct tm firstDay = timeInfo;

    firstDay.tm_mday = 1;
    firstDay.tm_hour = 12;
    firstDay.tm_min = 0;
    firstDay.tm_sec = 0;

    mktime(&firstDay);

    int firstWeekday =
        firstDay.tm_wday;

    // =================================================
    // Month Names
    // =================================================

    const char* monthNames[] = {

        "JANUARY",
        "FEBRUARY",
        "MARCH",
        "APRIL",
        "MAY",
        "JUNE",
        "JULY",
        "AUGUST",
        "SEPTEMBER",
        "OCTOBER",
        "NOVEMBER",
        "DECEMBER"
    };

    // =================================================
    // Clear Display
    // =================================================

    display->clearDisplay();

    display->setTextColor(
        SSD1306_WHITE
    );

    display->setTextSize(1);

    // =================================================
    // Header
    // =================================================

    String header =
        String(monthNames[month]) +
        " " +
        String(year);

    int headerWidth =
        header.length() * 6;

    int headerX =
        (128 - headerWidth) / 2;

    display->setCursor(
        headerX,
        0
    );

    display->println(
        header
    );

    // =================================================
    // Weekday Header
    // =================================================

    display->setCursor(
        25,
        10
    );

    display->print(
        "S M T W T F S"
    );

    // =================================================
    // Calendar Grid
    // =================================================

    const int startX = 22;
    const int startY = 20;

    const int cellWidth = 15;
    const int cellHeight = 8;

    for (
        int day = 1;
        day <= totalDays;
        day++
    ) {

        int position =
            firstWeekday +
            (day - 1);

        int column =
            position % 7;

        int row =
            position / 7;

        int x =
            startX +
            column * cellWidth;

        int y =
            startY +
            row * cellHeight;

        // =================================================
        // Highlight Today
        // =================================================

        if (day == today) {

            display->fillRoundRect(
                x - 1,
                y - 1,
                13,
                8,
                2,
                SSD1306_WHITE
            );

            display->setTextColor(
                SSD1306_BLACK
            );

        } else {

            display->setTextColor(
                SSD1306_WHITE
            );
        }

        // =================================================
        // Center Single Digit
        // =================================================

        if (day < 10) {

            display->setCursor(
                x + 3,
                y
            );

        } else {

            display->setCursor(
                x,
                y
            );
        }

        display->print(
            day
        );

        display->setTextColor(
            SSD1306_WHITE
        );
    }

    // =================================================
    // Show Display
    // =================================================

    display->display();
}