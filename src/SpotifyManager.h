#ifndef SPOTIFY_MANAGER_H
#define SPOTIFY_MANAGER_H

#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <WebServer.h>

class SpotifyManager {

public:

    SpotifyManager(
        Adafruit_SSD1306* display,
        WebServer* server,
        bool* spotifyMode
    );

    void updateData(
        String song,
        String artist,
        String album,
        bool playing,
        unsigned long progress,
        unsigned long duration
    );

    void drawScreen();

    void handleGET();

    void handlePOST();

private:

    Adafruit_SSD1306* display;
    WebServer* server;
    bool* spotifyMode;

    String spotifySong;
    String spotifyArtist;
    String spotifyAlbum;

    bool spotifyPlaying;

    unsigned long spotifyProgress;
    unsigned long spotifyDuration;

    unsigned long spotifyLastSync;

    String truncateText(
        String text,
        int maxLength
    );

    void drawEqualizer();

    unsigned long getDisplayProgress();
};

#endif