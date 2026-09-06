#include "SpotifyManager.h"


// =====================================================
// Constructor
// =====================================================

SpotifyManager::SpotifyManager(
    Adafruit_SSD1306* display,
    WebServer* server,
    bool* spotifyMode
) {

    this->display = display;
    this->server = server;
    this->spotifyMode = spotifyMode;

    spotifySong = "";
    spotifyArtist = "";
    spotifyAlbum = "";

    spotifyPlaying = false;

    spotifyProgress = 0;
    spotifyDuration = 0;

    spotifyLastSync = 0;
}


// =====================================================
// Truncate Text
// =====================================================

String SpotifyManager::truncateText(
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

void SpotifyManager::drawEqualizer() {

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


        display->fillRect(
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

unsigned long SpotifyManager::getDisplayProgress() {

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
        progress = spotifyDuration;
    }

    return progress;
}


// =====================================================
// Draw Spotify Screen
// =====================================================

void SpotifyManager::drawScreen() {

    display->clearDisplay();

    display->setTextColor(
        SSD1306_WHITE
    );

    display->setTextSize(1);


    if (spotifySong.length() == 0) {

        display->setCursor(
            0,
            0
        );

        display->println(
            "SPOTIFY"
        );

        display->setCursor(
            0,
            18
        );

        display->println(
            "Nothing playing"
        );

        display->display();

        return;
    }


    display->setCursor(
        0,
        0
    );

    display->println(
        truncateText(
            spotifySong,
            21
        )
    );


    display->setCursor(
        0,
        10
    );

    display->println(
        truncateText(
            spotifyArtist,
            21
        )
    );


    display->setCursor(
        118,
        0
    );

    if (spotifyPlaying) {
        display->print(">");
    } else {
        display->print("||");
    }


    drawEqualizer();


    unsigned long progress =
        getDisplayProgress();


    int barX = 2;
    int barY = 49;
    int barWidth = 124;
    int barHeight = 5;


    display->drawRect(
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

            display->fillRect(
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


    display->setCursor(
        0,
        57
    );


    if (progressMinutes < 10)
        display->print("0");

    display->print(
        progressMinutes
    );

    display->print(":");


    if (progressRemaining < 10)
        display->print("0");

    display->print(
        progressRemaining
    );

    display->print(
        " / "
    );


    if (durationMinutes < 10)
        display->print("0");

    display->print(
        durationMinutes
    );

    display->print(":");


    if (durationRemaining < 10)
        display->print("0");

    display->print(
        durationRemaining
    );


    display->display();
}


// =====================================================
// Update Spotify Data
// =====================================================

void SpotifyManager::updateData(
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


    // EXACT SAME BEHAVIOR AS OLD main.cpp

    if (*spotifyMode) {
        drawScreen();
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

void SpotifyManager::handleGET() {

    server->sendHeader(
        "Access-Control-Allow-Origin",
        "*"
    );


    if (
        !server->hasArg("song") ||
        !server->hasArg("artist") ||
        !server->hasArg("playing") ||
        !server->hasArg("progress") ||
        !server->hasArg("duration")
    ) {

        server->send(
            400,
            "text/plain",
            "Missing Spotify parameters"
        );

        return;
    }


    String song =
        server->arg("song");

    String artist =
        server->arg("artist");

    bool playing =
        server->arg("playing") == "true";

    unsigned long progress =
        server->arg("progress").toInt();

    unsigned long duration =
        server->arg("duration").toInt();


    String album = "";


    if (server->hasArg("album")) {

        album =
            server->arg("album");
    }


    updateData(
        song,
        artist,
        album,
        playing,
        progress,
        duration
    );


    server->send(
        200,
        "text/plain",
        "Spotify data received"
    );
}


// =====================================================
// Spotify POST Endpoint
// =====================================================

void SpotifyManager::handlePOST() {

    server->sendHeader(
        "Access-Control-Allow-Origin",
        "*"
    );


    if (!server->hasArg("plain")) {

        server->send(
            400,
            "text/plain",
            "Missing JSON body"
        );

        return;
    }


    String body =
        server->arg("plain");


    Serial.println();

    Serial.println(
        "===== Spotify POST ====="
    );

    Serial.println(
        body
    );


    // =================================================
    // Extract String
    // =================================================

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


    // =================================================
    // Extract Number
    // =================================================

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


    updateData(
        song,
        artist,
        album,
        playing,
        progress,
        duration
    );


    server->send(
        200,
        "text/plain",
        "Spotify data received"
    );
}