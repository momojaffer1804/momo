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
#include "taskmanager.h"
#include "SpotifyManager.h"
#include "TimeManager.h"
#include "CalendarManager.h"
#include "PomodoroManager.h"
#include "WebCommandManager.h"

// =====================================================
// Wi-Fi & WebServer Config
// =====================================================

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

WebServer server(80);

// =====================================================
// Laptop Configuration
// =====================================================

const char* LAPTOP_IP = "10.164.92.252";

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
TaskManager taskManager;

TimeManager timeManager(&display);
CalendarManager calendarManager(&display);

PomodoroManager pomodoroManager(
    &display,
    &pomodoro
);

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
bool todoMode = false;

// =====================================================
// Spotify Manager
// =====================================================

SpotifyManager spotify(
    &display,
    &server,
    &spotifyMode
);

// =====================================================
// Web Command Manager
// =====================================================

WebCommandManager webCommands(
    &server,
    &display,

    &pomodoro,
    &pomodoroManager,

    &taskManager,

    &spotify,

    &timeManager,
    &calendarManager,

    &eyes,
    &animation,
    &expressions,

    &currentExpression,

    &baseLeft,
    &baseRight,

    &spotifyMode,
    &calendarMode,
    &clockMode,
    &pomodoroMode,
    &todoMode
);

// =====================================================
// Setup
// =====================================================

void setup() {

    // =================================================
    // Serial
    // =================================================

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

    // =================================================
    // Wi-Fi Connection Result
    // =================================================

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

        // =================================================
        // Disable Wi-Fi Power Saving
        // =================================================

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

        // =================================================
        // Task Manager
        // =================================================

        taskManager.begin(
            String(LAPTOP_IP)
        );

        Serial.print(
            "Task server: http://"
        );

        Serial.print(
            LAPTOP_IP
        );

        Serial.println(
            ":5000"
        );

        // =================================================
        // Fetch Today's Tasks
        // =================================================

        taskManager.fetchTasks();
    }

    // =================================================
    // Web Command Routing
    // =================================================

    webCommands.begin();

    // =================================================
    // Start Web Server
    // =================================================

    server.begin();

    Serial.println(
        "Web server started."
    );

    // =================================================
    // Start Neutral Expression
    // =================================================

    currentExpression =
        EXPRESSION_NEUTRAL;

    expressions.setExpression(
        currentExpression
    );

    // Get initial eye states

    expressions.getExpressionStates(
        currentExpression,
        baseLeft,
        baseRight
    );

    // Draw initial eyes

    eyes.draw();
}

// =====================================================
// Main Loop
// =====================================================

void loop() {

    // =================================================
    // Handle HTTP Requests
    // =================================================

    server.handleClient();

    // =================================================
    // To-Do Mode
    // =================================================

    if (todoMode) {

        server.handleClient();

        delay(5);

        return;
    }

    // =================================================
    // Calendar Mode
    // =================================================

    if (calendarMode) {

        static unsigned long lastCalendarFrame = 0;

        if (
            millis() - lastCalendarFrame >= 1000
        ) {

            calendarManager.drawScreen();

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

            timeManager.drawScreen();

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

        // Update timer continuously

        pomodoro.update();

        if (
            millis() - lastPomodoroFrame >= 500
        ) {

            pomodoroManager.drawScreen();

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

            spotify.drawScreen();

            lastSpotifyFrame =
                millis();
        }

        delay(5);

        return;
    }

    // =================================================
    // Normal Eye Mode
    // =================================================

    webCommands.updateNormalEyes();

    delay(16);
}