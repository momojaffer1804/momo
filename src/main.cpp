#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WebServer.h>
#include <time.h>

#include "Pomodoro.h"
#include "EyeEngine.h"
#include "AnimationEngine.h"
#include "ExpressionEngine.h"

// =====================================================
// Wi-Fi & WebServer Config
// =====================================================

const char* ssid = "oneplus4";
const char* password = "Momos0786";

WebServer server(80);

// =====================================================
// OLED
// =====================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    &Wire,
    OLED_RESET
);

// =====================================================
// Engines
// =====================================================

EyeEngine eyes(&display);
AnimationEngine animation(&eyes);
ExpressionEngine expressions(&eyes);
Pomodoro pomodoro;

// =====================================================
// Current Expression
// =====================================================

Expression currentExpression =
    EXPRESSION_NEUTRAL;

// =====================================================
// Current Expression Eye States
// =====================================================

EyeState baseLeft;
EyeState baseRight;

// =====================================================
// Display Modes
// =====================================================

bool spotifyMode = false;
bool calendarMode = false;
bool clockMode = false;
bool pomodoroMode = false;

// =====================================================
// Spotify State
// =====================================================

String spotifySong = "";
String spotifyArtist = "";
String spotifyAlbum = "";

bool spotifyPlaying = false;

unsigned long spotifyProgress = 0;
unsigned long spotifyDuration = 0;

unsigned long spotifyLastSync = 0;

// =====================================================
// Get Current Expression State
// =====================================================

void updateBaseExpression() {

    expressions.getExpressionStates(
        currentExpression,
        baseLeft,
        baseRight
    );
}

// =====================================================
// Animate Eyes + Keep Current Mouth
// =====================================================

void animateEyeMovement(
    EyeState targetLeft,
    EyeState targetRight,
    unsigned long duration
) {

    MouthState currentMouth =
        eyes.getMouth();

    animation.moveTo(
        targetLeft,
        targetRight,
        currentMouth,
        duration
    );

    while (animation.isAnimating()) {

        animation.update();

        delay(16);
    }
}

// =====================================================
// Return To Current Expression
// =====================================================

void returnToExpression() {

    animateEyeMovement(
        baseLeft,
        baseRight,
        300
    );
}

// =====================================================
// Look Directions
// =====================================================

void lookLeft() {

    EyeState targetLeft = baseLeft;
    EyeState targetRight = baseRight;

    targetLeft.x -= 10;
    targetRight.x -= 10;

    animateEyeMovement(
        targetLeft,
        targetRight,
        300
    );

    delay(300);

    returnToExpression();
}

void lookRight() {

    EyeState targetLeft = baseLeft;
    EyeState targetRight = baseRight;

    targetLeft.x += 10;
    targetRight.x += 10;

    animateEyeMovement(
        targetLeft,
        targetRight,
        300
    );

    delay(300);

    returnToExpression();
}

void lookUp() {

    EyeState targetLeft = baseLeft;
    EyeState targetRight = baseRight;

    targetLeft.y -= 7;
    targetRight.y -= 7;

    animateEyeMovement(
        targetLeft,
        targetRight,
        300
    );

    delay(300);

    returnToExpression();
}

void lookDown() {

    EyeState targetLeft = baseLeft;
    EyeState targetRight = baseRight;

    targetLeft.y += 7;
    targetRight.y += 7;

    animateEyeMovement(
        targetLeft,
        targetRight,
        300
    );

    delay(300);

    returnToExpression();
}

// =====================================================
// Blink
// =====================================================

void blink() {

    EyeState closedLeft =
        eyes.getLeftEye();

    EyeState closedRight =
        eyes.getRightEye();

    closedLeft.height = 4;
    closedRight.height = 4;

    closedLeft.pupilSize = 0;
    closedRight.pupilSize = 0;

    MouthState currentMouth =
        eyes.getMouth();

    animation.moveTo(
        closedLeft,
        closedRight,
        currentMouth,
        90
    );

    while (animation.isAnimating()) {

        animation.update();

        delay(16);
    }

    delay(60);

    animation.moveTo(
        baseLeft,
        baseRight,
        currentMouth,
        110
    );

    while (animation.isAnimating()) {

        animation.update();

        delay(16);
    }
}

// =====================================================
// Show Expression
// =====================================================

void showExpression(
    Expression expression,
    unsigned long duration
) {

    EyeState targetLeft;
    EyeState targetRight;

    expressions.getExpressionStates(
        expression,
        targetLeft,
        targetRight
    );

    MouthState targetMouth =
        expressions.getExpressionMouth(
            expression
        );

    animation.moveTo(
        targetLeft,
        targetRight,
        targetMouth,
        duration
    );

    while (animation.isAnimating()) {

        animation.update();

        delay(16);
    }

    currentExpression =
        expression;

    baseLeft =
        targetLeft;

    baseRight =
        targetRight;

    expressions.setExpression(
        currentExpression
    );

    eyes.draw();
}

// =====================================================
// Spotify Display Helpers
// =====================================================

String truncateText(
    String text,
    int maxLength
) {

    if (text.length() <= maxLength) {

        return text;
    }

    return text.substring(
        0,
        maxLength - 3
    ) + "...";
}

// =====================================================
// Spotify Equalizer
// =====================================================

void drawSpotifyEqualizer() {

    const int barCount = 12;
    const int barWidth = 5;
    const int gap = 2;

    const int startX = 18;
    const int bottomY = 45;
    const int maxHeight = 20;

    const uint8_t levels[barCount] = {

        4, 8, 13, 18,
        15, 10, 6, 11,
        17, 13, 8, 4
    };

    unsigned long frame =
        spotifyPlaying
            ? (millis() / 120)
            : 0;

    for (
        int i = 0;
        i < barCount;
        i++
    ) {

        int level;

        if (spotifyPlaying) {

            int index =
                (
                    i +
                    (frame % barCount)
                )
                % barCount;

            level =
                levels[index];

            if (
                ((frame / 2) + i) % 5 == 0
            ) {

                level += 3;
            }

        } else {

            level =
                levels[i];
        }

        level =
            constrain(
                level,
                2,
                maxHeight
            );

        int x =
            startX +
            i * (barWidth + gap);

        int y =
            bottomY -
            level;

        display.fillRect(
            x,
            y,
            barWidth,
            level,
            SSD1306_WHITE
        );
    }
}

// =====================================================
// Get Display Progress
// =====================================================

unsigned long getDisplayProgress() {

    if (!spotifyPlaying) {

        return spotifyProgress;
    }

    if (spotifyDuration == 0) {

        return spotifyProgress;
    }

    unsigned long elapsed =
        millis() -
        spotifyLastSync;

    unsigned long progress =
        spotifyProgress +
        elapsed;

    if (progress > spotifyDuration) {

        progress =
            spotifyDuration;
    }

    return progress;
}

// =====================================================
// Draw Spotify Screen
// =====================================================

void drawSpotifyScreen() {

    display.clearDisplay();

    display.setTextColor(
        SSD1306_WHITE
    );

    display.setTextSize(1);

    if (spotifySong.length() == 0) {

        display.setCursor(
            0,
            0
        );

        display.println(
            "SPOTIFY"
        );

        display.setCursor(
            0,
            18
        );

        display.println(
            "Nothing playing"
        );

        display.display();

        return;
    }

    display.setCursor(
        0,
        0
    );

    display.println(
        truncateText(
            spotifySong,
            21
        )
    );

    display.setCursor(
        0,
        10
    );

    display.println(
        truncateText(
            spotifyArtist,
            21
        )
    );

    display.setCursor(
        118,
        0
    );

    if (spotifyPlaying) {

        display.print(">");

    } else {

        display.print("||");
    }

    drawSpotifyEqualizer();

    unsigned long progress =
        getDisplayProgress();

    int barX = 2;
    int barY = 49;
    int barWidth = 124;
    int barHeight = 5;

    display.drawRect(
        barX,
        barY,
        barWidth,
        barHeight,
        SSD1306_WHITE
    );

    if (spotifyDuration > 0) {

        int fillWidth =
            (long)(barWidth - 2) *
            progress /
            spotifyDuration;

        fillWidth =
            constrain(
                fillWidth,
                0,
                barWidth - 2
            );

        if (fillWidth > 0) {

            display.fillRect(
                barX + 1,
                barY + 1,
                fillWidth,
                barHeight - 2,
                SSD1306_WHITE
            );
        }
    }

    unsigned long progressSeconds =
        progress / 1000;

    unsigned long durationSeconds =
        spotifyDuration / 1000;

    unsigned long progressMinutes =
        progressSeconds / 60;

    unsigned long progressRemaining =
        progressSeconds % 60;

    unsigned long durationMinutes =
        durationSeconds / 60;

    unsigned long durationRemaining =
        durationSeconds % 60;

    display.setCursor(
        0,
        57
    );

    if (progressMinutes < 10)
        display.print("0");

    display.print(
        progressMinutes
    );

    display.print(":");

    if (progressRemaining < 10)
        display.print("0");

    display.print(
        progressRemaining
    );

    display.print(
        " / "
    );

    if (durationMinutes < 10)
        display.print("0");

    display.print(
        durationMinutes
    );

    display.print(":");

    if (durationRemaining < 10)
        display.print("0");

    display.print(
        durationRemaining
    );

    display.display();
}

// =====================================================
// Draw Calendar Screen
// =====================================================

void drawCalendarScreen() {

    struct tm timeInfo;

    if (!getLocalTime(&timeInfo)) {

        display.clearDisplay();

        display.setTextColor(
            SSD1306_WHITE
        );

        display.setTextSize(1);

        display.setCursor(
            38,
            25
        );

        display.println(
            "Loading..."
        );

        display.display();

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

    struct tm firstDay = timeInfo;

    firstDay.tm_mday = 1;
    firstDay.tm_hour = 12;
    firstDay.tm_min = 0;
    firstDay.tm_sec = 0;

    mktime(&firstDay);

    int firstWeekday =
        firstDay.tm_wday;

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

    display.clearDisplay();

    display.setTextColor(
        SSD1306_WHITE
    );

    display.setTextSize(1);

    String header =
        String(monthNames[month]) +
        " " +
        String(year);

    int headerWidth =
        header.length() * 6;

    int headerX =
        (128 - headerWidth) / 2;

    display.setCursor(
        headerX,
        0
    );

    display.println(
        header
    );

    display.setCursor(
        25,
        10
    );

    display.print(
        "S M T W T F S"
    );

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

        if (day == today) {

            display.fillRoundRect(
                x - 1,
                y - 1,
                13,
                8,
                2,
                SSD1306_WHITE
            );

            display.setTextColor(
                SSD1306_BLACK
            );

        } else {

            display.setTextColor(
                SSD1306_WHITE
            );
        }

        if (day < 10) {

            display.setCursor(
                x + 3,
                y
            );

        } else {

            display.setCursor(
                x,
                y
            );
        }

        display.print(
            day
        );

        display.setTextColor(
            SSD1306_WHITE
        );
    }

    display.display();
}

// =====================================================
// Draw Clock Screen
// =====================================================

void drawClockScreen() {

    struct tm timeInfo;

    if (!getLocalTime(&timeInfo)) {

        display.clearDisplay();

        display.setTextColor(
            SSD1306_WHITE
        );

        display.setTextSize(1);

        display.setCursor(
            42,
            25
        );

        display.println(
            "Loading..."
        );

        display.display();

        return;
    }

    display.clearDisplay();

    display.setTextColor(
        SSD1306_WHITE
    );

    char timeString[12];

    strftime(
        timeString,
        sizeof(timeString),
        "%I:%M %p",
        &timeInfo
    );

    display.setTextSize(2);

    int timeWidth =
        strlen(timeString) * 12;

    int timeX =
        (128 - timeWidth) / 2;

    display.setCursor(
        timeX,
        4
    );

    display.println(
        timeString
    );

    char dayString[15];

    strftime(
        dayString,
        sizeof(dayString),
        "%A",
        &timeInfo
    );

    display.setTextSize(1);

    int dayWidth =
        strlen(dayString) * 6;

    int dayX =
        (128 - dayWidth) / 2;

    display.setCursor(
        dayX,
        30
    );

    display.println(
        dayString
    );

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

    display.setCursor(
        dateX,
        45
    );

    display.println(
        dateString
    );

    display.display();
}

// =====================================================
// Draw Pomodoro Screen
// =====================================================

void drawPomodoroScreen() {

    display.clearDisplay();

    display.setTextColor(
        SSD1306_WHITE
    );

    display.setTextSize(1);

    const char* title;

    if (
        pomodoro.getState() ==
        POMODORO_FOCUS
    ) {

        title = "FOCUS";

    } else {

        title = "BREAK";
    }

    int titleWidth =
        strlen(title) * 6;

    display.setCursor(
        (128 - titleWidth) / 2,
        0
    );

    display.println(
        title
    );

    unsigned long totalSeconds =
        pomodoro.getRemainingSeconds();

    unsigned long minutes =
        totalSeconds / 60;

    unsigned long seconds =
        totalSeconds % 60;

    char timerString[6];

    snprintf(
        timerString,
        sizeof(timerString),
        "%02lu:%02lu",
        minutes,
        seconds
    );

    display.setTextSize(3);

    int timerWidth =
        strlen(timerString) * 18;

    display.setCursor(
        (128 - timerWidth) / 2,
        18
    );

    display.println(
        timerString
    );

    display.setTextSize(1);

    const char* status;

    if (pomodoro.isRunning()) {

        status = "RUNNING";

    } else {

        status = "PAUSED";
    }

    int statusWidth =
        strlen(status) * 6;

    display.setCursor(
        (128 - statusWidth) / 2,
        50
    );

    display.println(
        status
    );

    display.display();
}

// =====================================================
// Exit Spotify Mode
// =====================================================

void exitSpotifyMode() {

    spotifyMode = false;

    expressions.setExpression(
        currentExpression
    );

    eyes.setEyes(
        baseLeft,
        baseRight
    );

    eyes.draw();
}

// =====================================================
// Exit Calendar Mode
// =====================================================

void exitCalendarMode() {

    calendarMode = false;

    expressions.setExpression(
        currentExpression
    );

    eyes.setEyes(
        baseLeft,
        baseRight
    );

    eyes.draw();
}

// =====================================================
// Exit Clock Mode
// =====================================================

void exitClockMode() {

    clockMode = false;

    expressions.setExpression(
        currentExpression
    );

    eyes.setEyes(
        baseLeft,
        baseRight
    );

    eyes.draw();
}

// =====================================================
// Exit Pomodoro Mode
// =====================================================

void exitPomodoroMode() {

    pomodoroMode = false;

    expressions.setExpression(
        currentExpression
    );

    eyes.setEyes(
        baseLeft,
        baseRight
    );

    eyes.draw();
}

// =====================================================
// Return To Normal Mode
// =====================================================

void returnToNormalMode() {

    spotifyMode = false;
    calendarMode = false;
    clockMode = false;
    pomodoroMode = false;

    expressions.setExpression(
        currentExpression
    );

    eyes.setEyes(
        baseLeft,
        baseRight
    );

    eyes.draw();
}

// =====================================================
// Apply Spotify Data
// =====================================================

void updateSpotifyData(
    String song,
    String artist,
    String album,
    bool playing,
    unsigned long progress,
    unsigned long duration
) {

    spotifySong =
        song;

    spotifyArtist =
        artist;

    spotifyAlbum =
        album;

    spotifyPlaying =
        playing;

    spotifyProgress =
        progress;

    spotifyDuration =
        duration;

    spotifyLastSync =
        millis();

    if (spotifyMode) {

        drawSpotifyScreen();
    }

    Serial.println();

    Serial.println(
        "========== SPOTIFY =========="
    );

    Serial.print(
        "Song     : "
    );

    Serial.println(
        spotifySong
    );

    Serial.print(
        "Artist   : "
    );

    Serial.println(
        spotifyArtist
    );

    Serial.print(
        "Album    : "
    );

    Serial.println(
        spotifyAlbum
    );

    Serial.print(
        "Playing  : "
    );

    Serial.println(
        spotifyPlaying
            ? "TRUE"
            : "FALSE"
    );

    Serial.print(
        "Progress : "
    );

    Serial.print(
        spotifyProgress
    );

    Serial.print(
        " / "
    );

    Serial.println(
        spotifyDuration
    );

    Serial.println(
        "============================="
    );
}

// =====================================================
// Spotify GET Endpoint
// =====================================================

void handleSpotifyGET() {

    server.sendHeader(
        "Access-Control-Allow-Origin",
        "*"
    );

    if (
        !server.hasArg("song") ||
        !server.hasArg("artist") ||
        !server.hasArg("playing") ||
        !server.hasArg("progress") ||
        !server.hasArg("duration")
    ) {

        server.send(
            400,
            "text/plain",
            "Missing Spotify parameters"
        );

        return;
    }

    String song =
        server.arg("song");

    String artist =
        server.arg("artist");

    bool playing =
        server.arg("playing") == "true";

    unsigned long progress =
        server.arg("progress").toInt();

    unsigned long duration =
        server.arg("duration").toInt();

    String album = "";

    if (server.hasArg("album")) {

        album =
            server.arg("album");
    }

    updateSpotifyData(
        song,
        artist,
        album,
        playing,
        progress,
        duration
    );

    server.send(
        200,
        "text/plain",
        "Spotify data received"
    );
}

// =====================================================
// Spotify POST Endpoint
// =====================================================

void handleSpotifyPOST() {

    server.sendHeader(
        "Access-Control-Allow-Origin",
        "*"
    );

    if (!server.hasArg("plain")) {

        server.send(
            400,
            "text/plain",
            "Missing JSON body"
        );

        return;
    }

    String body =
        server.arg("plain");

    Serial.println();

    Serial.println(
        "===== Spotify POST ====="
    );

    Serial.println(
        body
    );

    auto extractString =
        [&](const String& key) -> String {

            String search =
                "\"" + key + "\"";

            int keyIndex =
                body.indexOf(search);

            if (keyIndex < 0)
                return "";

            int colon =
                body.indexOf(
                    ":",
                    keyIndex
                );

            if (colon < 0)
                return "";

            int firstQuote =
                body.indexOf(
                    "\"",
                    colon + 1
                );

            if (firstQuote < 0)
                return "";

            int secondQuote =
                body.indexOf(
                    "\"",
                    firstQuote + 1
                );

            if (secondQuote < 0)
                return "";

            return body.substring(
                firstQuote + 1,
                secondQuote
            );
        };

    auto extractNumber =
        [&](const String& key) -> unsigned long {

            String search =
                "\"" + key + "\"";

            int keyIndex =
                body.indexOf(search);

            if (keyIndex < 0)
                return 0;

            int colon =
                body.indexOf(
                    ":",
                    keyIndex
                );

            if (colon < 0)
                return 0;

            int start =
                colon + 1;

            while (
                start < body.length() &&
                body[start] == ' '
            ) {

                start++;
            }

            int end =
                start;

            while (
                end < body.length() &&
                isDigit(body[end])
            ) {

                end++;
            }

            return body.substring(
                start,
                end
            ).toInt();
        };

    String song =
        extractString("name");

    String artist =
        extractString("artist");

    String album =
        extractString("album");

    unsigned long progress =
        extractNumber("progress_ms");

    unsigned long duration =
        extractNumber("duration_ms");

    bool playing =
        false;

    int playingIndex =
        body.indexOf(
            "\"is_playing\""
        );

    if (playingIndex >= 0) {

        int colon =
            body.indexOf(
                ":",
                playingIndex
            );

        if (colon >= 0) {

            String value =
                body.substring(
                    colon + 1,
                    colon + 10
                );

            value.trim();

            playing =
                value.startsWith("true");
        }
    }

    updateSpotifyData(
        song,
        artist,
        album,
        playing,
        progress,
        duration
    );

    server.send(
        200,
        "text/plain",
        "Spotify data received"
    );
}

// =====================================================
// Web API Routing
// =====================================================

void setupRouting() {

    server.on(
        "/action",
        HTTP_OPTIONS,
        []() {

            server.sendHeader(
                "Access-Control-Allow-Origin",
                "*"
            );

            server.sendHeader(
                "Access-Control-Allow-Methods",
                "GET, POST, OPTIONS"
            );

            server.sendHeader(
                "Access-Control-Allow-Headers",
                "Content-Type"
            );

            server.send(204);
        }
    );

    server.on(
        "/spotify",
        HTTP_OPTIONS,
        []() {

            server.sendHeader(
                "Access-Control-Allow-Origin",
                "*"
            );

            server.sendHeader(
                "Access-Control-Allow-Methods",
                "GET, POST, OPTIONS"
            );

            server.sendHeader(
                "Access-Control-Allow-Headers",
                "Content-Type"
            );

            server.send(204);
        }
    );

    server.on(
        "/spotify",
        HTTP_GET,
        handleSpotifyGET
    );

    server.on(
        "/spotify",
        HTTP_POST,
        handleSpotifyPOST
    );

    server.on(
        "/action",
        HTTP_GET,
        []() {

            server.sendHeader(
                "Access-Control-Allow-Origin",
                "*"
            );

            if (!server.hasArg("cmd")) {

                server.send(
                    400,
                    "text/plain",
                    "Error: Missing 'cmd' argument"
                );

                return;
            }

            String cmd =
                server.arg("cmd");

            cmd.toUpperCase();
            cmd.trim();

            Serial.print(
                "API Command Received: "
            );

            Serial.println(
                cmd
            );

            // =========================================
            // Expressions
            // =========================================

            if (cmd == "HAPPY") {

                returnToNormalMode();

                showExpression(
                    EXPRESSION_HAPPY,
                    500
                );
            }

            else if (cmd == "SAD") {

                returnToNormalMode();

                showExpression(
                    EXPRESSION_SAD,
                    500
                );
            }

            else if (cmd == "ANGRY") {

                returnToNormalMode();

                showExpression(
                    EXPRESSION_ANGRY,
                    500
                );
            }

            else if (
                cmd == "SURPRISED" ||
                cmd == "SHOCK"
            ) {

                returnToNormalMode();

                showExpression(
                    EXPRESSION_SURPRISED,
                    500
                );
            }

            else if (cmd == "SLEEPY") {

                returnToNormalMode();

                showExpression(
                    EXPRESSION_SLEEPY,
                    500
                );
            }

            else if (
                cmd == "NEUTRAL" ||
                cmd == "RESET"
            ) {

                returnToNormalMode();

                showExpression(
                    EXPRESSION_NEUTRAL,
                    500
                );
            }

            // =========================================
            // Movement
            // =========================================

            else if (
                cmd == "LOOK_LEFT" ||
                cmd == "LEFT"
            ) {

                if (
                    !spotifyMode &&
                    !calendarMode &&
                    !clockMode &&
                    !pomodoroMode
                ) {

                    lookLeft();
                }
            }

            else if (
                cmd == "LOOK_RIGHT" ||
                cmd == "RIGHT"
            ) {

                if (
                    !spotifyMode &&
                    !calendarMode &&
                    !clockMode &&
                    !pomodoroMode
                ) {

                    lookRight();
                }
            }

            else if (
                cmd == "LOOK_UP" ||
                cmd == "UP"
            ) {

                if (
                    !spotifyMode &&
                    !calendarMode &&
                    !clockMode &&
                    !pomodoroMode
                ) {

                    lookUp();
                }
            }

            else if (
                cmd == "LOOK_DOWN" ||
                cmd == "DOWN"
            ) {

                if (
                    !spotifyMode &&
                    !calendarMode &&
                    !clockMode &&
                    !pomodoroMode
                ) {

                    lookDown();
                }
            }

            else if (
                cmd == "BLINK" ||
                cmd == "WINK"
            ) {

                if (
                    !spotifyMode &&
                    !calendarMode &&
                    !clockMode &&
                    !pomodoroMode
                ) {

                    blink();
                }
            }

            // =========================================
            // Calendar
            // =========================================

            else if (
                cmd == "CALENDAR" ||
                cmd == "1"
            ) {

                spotifyMode = false;
                clockMode = false;
                pomodoroMode = false;
                calendarMode = true;

                drawCalendarScreen();
            }

            // =========================================
            // Clock / Time
            // =========================================

            else if (
                cmd == "TIME" ||
                cmd == "CLOCK" ||
                cmd == "2"
            ) {

                spotifyMode = false;
                calendarMode = false;
                pomodoroMode = false;
                clockMode = true;

                drawClockScreen();
            }

            // =========================================
            // Spotify
            // =========================================

            else if (
                cmd == "SPOTIFY" ||
                cmd == "3"
            ) {

                calendarMode = false;
                clockMode = false;
                pomodoroMode = false;
                spotifyMode = true;

                drawSpotifyScreen();
            }

            // =========================================
            // Pomodoro Screen
            // =========================================

            else if (
                cmd == "POMODORO" ||
                cmd == "6"
            ) {

                spotifyMode = false;
                calendarMode = false;
                clockMode = false;
                pomodoroMode = true;

                drawPomodoroScreen();
            }

            // =========================================
            // Pomodoro Start / Pause
            // =========================================

            else if (
                cmd == "POMODORO_START" ||
                cmd == "4"
            ) {

                spotifyMode = false;
                calendarMode = false;
                clockMode = false;
                pomodoroMode = true;

                pomodoro.startPause();

                drawPomodoroScreen();
            }

            // =========================================
            // Pomodoro Reset
            // =========================================

            else if (
                cmd == "POMODORO_RESET" ||
                cmd == "5"
            ) {

                spotifyMode = false;
                calendarMode = false;
                clockMode = false;
                pomodoroMode = true;

                pomodoro.reset();

                drawPomodoroScreen();
            }

            // =========================================
            // NULL / NORMAL EYES
            // =========================================

            else if (
                cmd == "NULL" ||
                cmd == "0"
            ) {

                returnToNormalMode();

                Serial.println(
                    "Switched to NULL / normal eyes."
                );
            }

            // =========================================
            // Exit Spotify
            // =========================================

            else if (
                cmd == "EXIT_SPOTIFY" ||
                cmd == "EXIT"
            ) {

                returnToNormalMode();
            }

            // =========================================
            // Unknown Command
            // =========================================

            else {

                server.send(
                    400,
                    "text/plain",
                    "Error: Unknown command '" +
                    cmd +
                    "'"
                );

                return;
            }

            server.send(
                200,
                "text/plain",
                "Command Executed: " +
                cmd
            );
        }
    );
}

// =====================================================
// Setup
// =====================================================

void setup() {

    Serial.begin(115200);

    // =================================================
    // I2C
    // =================================================

    Wire.begin(
        21,
        22
    );

    // =================================================
    // OLED
    // =================================================

    if (
        !display.begin(
            SSD1306_SWITCHCAPVCC,
            SCREEN_ADDRESS
        )
    ) {

        Serial.println(
            "OLED not found!"
        );

        while (true) {

            delay(1000);
        }
    }

    Serial.println(
        "MoMo Eye Engine V2 Started"
    );

    // =================================================
    // Wi-Fi
    // =================================================

    Serial.print(
        "Connecting to Wi-Fi"
    );

    WiFi.mode(
        WIFI_STA
    );

    WiFi.disconnect();

    delay(100);

    WiFi.begin(
        ssid,
        password
    );

    int timeout = 0;

    while (
        WiFi.status() != WL_CONNECTED &&
        timeout < 30
    ) {

        delay(1000);

        timeout++;

        Serial.print(
            "Status code: "
        );

        Serial.println(
            WiFi.status()
        );
    }

    if (
        WiFi.status() != WL_CONNECTED
    ) {

        Serial.println(
            "\n--- WiFi Connection Failed ---"
        );

    } else {

        Serial.println(
            "\nConnected successfully!"
        );

        Serial.print(
            "IP: "
        );

        Serial.println(
            WiFi.localIP()
        );

        WiFi.setSleep(false);

        // =================================================
        // NTP Time Synchronization - IST
        // =================================================

        configTime(
            19800,
            0,
            "pool.ntp.org",
            "time.nist.gov"
        );

        Serial.println(
            "NTP time synchronization started."
        );
    }

    // =================================================
    // Web Server
    // =================================================

    setupRouting();

    server.begin();

    Serial.println(
        "Web server started."
    );

    // =================================================
    // Start Neutral
    // =================================================

    currentExpression =
        EXPRESSION_NEUTRAL;

    expressions.setExpression(
        currentExpression
    );

    updateBaseExpression();

    eyes.draw();
}

// =====================================================
// Main Loop
// =====================================================

void loop() {

    // =================================================
    // Handle HTTP
    // =================================================

    server.handleClient();

    // =================================================
    // Calendar Mode
    // =================================================

    if (calendarMode) {

        static unsigned long lastCalendarFrame = 0;

        if (
            millis() - lastCalendarFrame >= 1000
        ) {

            drawCalendarScreen();

            lastCalendarFrame =
                millis();
        }

        delay(5);

        return;
    }

    // =================================================
    // Clock Mode
    // =================================================

    if (clockMode) {

        static unsigned long lastClockFrame = 0;

        if (
            millis() - lastClockFrame >= 500
        ) {

            drawClockScreen();

            lastClockFrame =
                millis();
        }

        delay(5);

        return;
    }

    // =================================================
    // Pomodoro Mode
    // =================================================

    if (pomodoroMode) {

        static unsigned long lastPomodoroFrame = 0;

        pomodoro.update();

        if (
            millis() - lastPomodoroFrame >= 500
        ) {

            drawPomodoroScreen();

            lastPomodoroFrame =
                millis();
        }

        delay(5);

        return;
    }

    // =================================================
    // Spotify Mode
    // =================================================

    if (spotifyMode) {

        static unsigned long lastSpotifyFrame = 0;

        if (
            millis() - lastSpotifyFrame >= 80
        ) {

            drawSpotifyScreen();

            lastSpotifyFrame =
                millis();
        }

        delay(5);

        return;
    }

    // =================================================
    // Normal Eye Mode
    // =================================================

    static unsigned long lastBlink = 0;
    static unsigned long lastLook = 0;

    unsigned long now =
        millis();

    if (
        now - lastBlink > 3500
    ) {

        blink();

        lastBlink =
            millis();
    }

    if (
        now - lastLook > 7000
    ) {

        int movement =
            random(0, 4);

        if (movement == 0) {

            lookLeft();
        }

        else if (movement == 1) {

            lookRight();
        }

        else if (movement == 2) {

            lookUp();
        }

        else {

            lookDown();
        }

        lastLook =
            millis();
    }

    delay(16);
}