#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WebServer.h>

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

// =====================================================
// Current Expression
// =====================================================

Expression currentExpression = EXPRESSION_NEUTRAL;

// =====================================================
// Current Expression Eye States
// =====================================================

EyeState baseLeft;
EyeState baseRight;

// =====================================================
// Spotify State
// =====================================================

String spotifySong = "";
String spotifyArtist = "";
String spotifyAlbum = "";

bool spotifyPlaying = false;

unsigned long spotifyProgress = 0;
unsigned long spotifyDuration = 0;

// Time when latest Spotify update was received
unsigned long spotifyLastSync = 0;

bool spotifyMode = false;

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
    MouthState currentMouth = eyes.getMouth();

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
    EyeState closedLeft = eyes.getLeftEye();
    EyeState closedRight = eyes.getRightEye();

    closedLeft.height = 4;
    closedRight.height = 4;

    closedLeft.pupilSize = 0;
    closedRight.pupilSize = 0;

    MouthState currentMouth = eyes.getMouth();

    // Close
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

    // Open
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

    currentExpression = expression;

    baseLeft = targetLeft;
    baseRight = targetRight;

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

    return text.substring(0, maxLength - 3) + "...";
}

// =====================================================
// Spotify Equalizer
// =====================================================
//
// Playing:
//     Animated bars
//
// Paused:
//     Static bars
//
// This is a visual animation, not actual audio spectrum data.
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

    for (int i = 0; i < barCount; i++) {

        int level;

        if (spotifyPlaying) {

            int index =
                (i + (frame % barCount))
                % barCount;

            level = levels[index];

            if (((frame / 2) + i) % 5 == 0) {
                level += 3;
            }

        } else {

            // Completely static while paused
            level = levels[i];
        }

        level = constrain(
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
        millis() - spotifyLastSync;

    unsigned long progress =
        spotifyProgress + elapsed;

    if (progress > spotifyDuration) {
        progress = spotifyDuration;
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

    // =================================================
    // Nothing Playing
    // =================================================

    if (spotifySong.length() == 0) {

        display.setCursor(0, 0);
        display.println("SPOTIFY");

        display.setCursor(0, 18);
        display.println("Nothing playing");

        display.display();

        return;
    }

    // =================================================
    // Song
    // =================================================

    display.setCursor(0, 0);

    display.println(
        truncateText(
            spotifySong,
            21
        )
    );

    // =================================================
    // Artist
    // =================================================

    display.setCursor(0, 10);

    display.println(
        truncateText(
            spotifyArtist,
            21
        )
    );

    // =================================================
    // Playing / Paused Indicator
    // =================================================

    display.setCursor(118, 0);

    if (spotifyPlaying) {
        display.print(">");
    } else {
        display.print("||");
    }

    // =================================================
    // Equalizer
    // =================================================

    drawSpotifyEqualizer();

    // =================================================
    // Progress
    // =================================================

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

        fillWidth = constrain(
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

    // =================================================
    // Time
    // =================================================

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

    display.setCursor(0, 57);

    if (progressMinutes < 10)
        display.print("0");

    display.print(progressMinutes);

    display.print(":");

    if (progressRemaining < 10)
        display.print("0");

    display.print(progressRemaining);

    display.print(" / ");

    if (durationMinutes < 10)
        display.print("0");

    display.print(durationMinutes);

    display.print(":");

    if (durationRemaining < 10)
        display.print("0");

    display.print(durationRemaining);

    display.display();
}

// =====================================================
// Exit Spotify Mode
// =====================================================

void exitSpotifyMode() {

    spotifyMode = false;

    spotifySong = "";
    spotifyArtist = "";
    spotifyAlbum = "";

    spotifyPlaying = false;

    spotifyProgress = 0;
    spotifyDuration = 0;

    spotifyLastSync = 0;

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

    spotifySong = song;
    spotifyArtist = artist;
    spotifyAlbum = album;

    spotifyPlaying = playing;

    spotifyProgress = progress;
    spotifyDuration = duration;

    spotifyLastSync = millis();

    spotifyMode = true;

    drawSpotifyScreen();

    Serial.println();
    Serial.println("========== SPOTIFY ==========");

    Serial.print("Song     : ");
    Serial.println(spotifySong);

    Serial.print("Artist   : ");
    Serial.println(spotifyArtist);

    Serial.print("Album    : ");
    Serial.println(spotifyAlbum);

    Serial.print("Playing  : ");
    Serial.println(
        spotifyPlaying
            ? "TRUE"
            : "FALSE"
    );

    Serial.print("Progress : ");
    Serial.print(spotifyProgress);
    Serial.print(" / ");
    Serial.println(spotifyDuration);

    Serial.println("=============================");
}

// =====================================================
// Spotify GET Endpoint
// =====================================================
//
// This is the important fix.
//
// Your Python bridge sends:
//
// /spotify?song=Waves&artist=Joey+Bada%24%24
// &playing=true
// &progress=119458
// &duration=211630
//
// =====================================================

void handleSpotifyGET() {

    server.sendHeader(
        "Access-Control-Allow-Origin",
        "*"
    );

    // -------------------------------------------------
    // Validate parameters
    // -------------------------------------------------

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

    // -------------------------------------------------
    // Read parameters
    // -------------------------------------------------

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

    // Album is optional for GET
    String album = "";

    if (server.hasArg("album")) {
        album = server.arg("album");
    }

    // -------------------------------------------------
    // Update Spotify
    // -------------------------------------------------

    updateSpotifyData(
        song,
        artist,
        album,
        playing,
        progress,
        duration
    );

    // -------------------------------------------------
    // Response
    // -------------------------------------------------

    server.send(
        200,
        "text/plain",
        "Spotify data received"
    );
}

// =====================================================
// Spotify POST Endpoint
// =====================================================
//
// Kept for compatibility if we later switch the Python
// bridge to JSON POST requests.
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
    Serial.println("===== Spotify POST =====");
    Serial.println(body);

    // -------------------------------------------------
    // Extract String
    // -------------------------------------------------

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

    // -------------------------------------------------
    // Extract Number
    // -------------------------------------------------

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

            int end = start;

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

    // -------------------------------------------------
    // Extract Data
    // -------------------------------------------------

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

    bool playing = false;

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

    // =================================================
    // ACTION OPTIONS
    // =================================================

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

    // =================================================
    // SPOTIFY OPTIONS
    // =================================================

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

    // =================================================
    // SPOTIFY GET
    // =================================================
    //
    // This matches your current spotify.py
    //
    // =================================================

    server.on(
        "/spotify",
        HTTP_GET,
        handleSpotifyGET
    );

    // =================================================
    // SPOTIFY POST
    // =================================================

    server.on(
        "/spotify",
        HTTP_POST,
        handleSpotifyPOST
    );

    // =================================================
    // ACTION GET
    // =================================================

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

            Serial.println(cmd);

            // =========================================
            // Expressions
            // =========================================

            if (cmd == "HAPPY") {

                exitSpotifyMode();

                showExpression(
                    EXPRESSION_HAPPY,
                    500
                );
            }

            else if (cmd == "SAD") {

                exitSpotifyMode();

                showExpression(
                    EXPRESSION_SAD,
                    500
                );
            }

            else if (cmd == "ANGRY") {

                exitSpotifyMode();

                showExpression(
                    EXPRESSION_ANGRY,
                    500
                );
            }

            else if (
                cmd == "SURPRISED" ||
                cmd == "SHOCK"
            ) {

                exitSpotifyMode();

                showExpression(
                    EXPRESSION_SURPRISED,
                    500
                );
            }

            else if (cmd == "SLEEPY") {

                exitSpotifyMode();

                showExpression(
                    EXPRESSION_SLEEPY,
                    500
                );
            }

            else if (
                cmd == "NEUTRAL" ||
                cmd == "RESET"
            ) {

                exitSpotifyMode();

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

                if (!spotifyMode)
                    lookLeft();
            }

            else if (
                cmd == "LOOK_RIGHT" ||
                cmd == "RIGHT"
            ) {

                if (!spotifyMode)
                    lookRight();
            }

            else if (
                cmd == "LOOK_UP" ||
                cmd == "UP"
            ) {

                if (!spotifyMode)
                    lookUp();
            }

            else if (
                cmd == "LOOK_DOWN" ||
                cmd == "DOWN"
            ) {

                if (!spotifyMode)
                    lookDown();
            }

            else if (
                cmd == "BLINK" ||
                cmd == "WINK"
            ) {

                if (!spotifyMode)
                    blink();
            }

            // =========================================
            // Spotify
            // =========================================

            else if (
                cmd == "SPOTIFY" ||
                cmd == "3"
            ) {

                spotifyMode = true;

                drawSpotifyScreen();
            }

            // =========================================
            // NULL / NORMAL EYES
            // =========================================

            else if (
                cmd == "NULL" ||
                cmd == "0"
            ) {

                exitSpotifyMode();

                Serial.println(
                    "Switched to NULL / normal eyes."
                );
            }

            else if (
                cmd == "EXIT_SPOTIFY" ||
                cmd == "EXIT"
            ) {

                exitSpotifyMode();
            }

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
                "Command Executed: " + cmd
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

    WiFi.mode(WIFI_STA);

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

    // =================================================
    // Idle Blink
    // =================================================

    if (
        now - lastBlink > 3500
    ) {

        blink();

        lastBlink =
            millis();
    }

    // =================================================
    // Idle Eye Movement
    // =================================================

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