#ifndef WEB_COMMAND_MANAGER_H
#define WEB_COMMAND_MANAGER_H

#include <Arduino.h>
#include <WebServer.h>
#include <Adafruit_SSD1306.h>

#include "Pomodoro.h"
#include "PomodoroManager.h"
#include "EyeEngine.h"
#include "AnimationEngine.h"
#include "ExpressionEngine.h"
#include "taskmanager.h"
#include "SpotifyManager.h"
#include "TimeManager.h"
#include "CalendarManager.h"

class WebCommandManager {

public:

    WebCommandManager(
        WebServer* server,
        Adafruit_SSD1306* display,
        Pomodoro* pomodoro,
        PomodoroManager* pomodoroManager,
        TaskManager* taskManager,
        SpotifyManager* spotify,
        TimeManager* timeManager,
        CalendarManager* calendarManager,
        EyeEngine* eyes,
        AnimationEngine* animation,
        ExpressionEngine* expressions,
        Expression* currentExpression,
        EyeState* baseLeft,
        EyeState* baseRight,
        bool* spotifyMode,
        bool* calendarMode,
        bool* clockMode,
        bool* pomodoroMode,
        bool* todoMode
    );

    // Public functions
    void begin();

    // Used by main.cpp
    void updateNormalEyes();


private:

    // =================================================
    // Pointers
    // =================================================

    WebServer* server;
    Adafruit_SSD1306* display;

    Pomodoro* pomodoro;
    PomodoroManager* pomodoroManager;

    TaskManager* taskManager;

    SpotifyManager* spotify;

    TimeManager* timeManager;
    CalendarManager* calendarManager;

    EyeEngine* eyes;
    AnimationEngine* animation;
    ExpressionEngine* expressions;

    Expression* currentExpression;

    EyeState* baseLeft;
    EyeState* baseRight;

    bool* spotifyMode;
    bool* calendarMode;
    bool* clockMode;
    bool* pomodoroMode;
    bool* todoMode;


    // =================================================
    // Web Routing
    // =================================================

    void setupRouting();


    // =================================================
    // Expression / Eye Functions
    // =================================================

    void updateBaseExpression();

    void animateEyeMovement(
        EyeState targetLeft,
        EyeState targetRight,
        unsigned long duration
    );

    void returnToExpression();

    void lookLeft();
    void lookRight();
    void lookUp();
    void lookDown();

    void blink();

    void showExpression(
        Expression expression,
        unsigned long duration
    );


    // =================================================
    // To-Do
    // =================================================

    void drawTodoScreen();

    void todoCelebration();


    // =================================================
    // Mode Exit
    // =================================================

    void exitSpotifyMode();
    void exitCalendarMode();
    void exitClockMode();
    void exitPomodoroMode();
    void exitTodoMode();

    void returnToNormalMode();
};

#endif