#include "WebCommandManager.h"

// =====================================================
// Constructor
// =====================================================

WebCommandManager::WebCommandManager(
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
) {
    this->server = server;
    this->display = display;

    this->pomodoro = pomodoro;
    this->pomodoroManager = pomodoroManager;

    this->taskManager = taskManager;
    this->spotify = spotify;

    this->timeManager = timeManager;
    this->calendarManager = calendarManager;

    this->eyes = eyes;
    this->animation = animation;
    this->expressions = expressions;

    this->currentExpression = currentExpression;

    this->baseLeft = baseLeft;
    this->baseRight = baseRight;

    this->spotifyMode = spotifyMode;
    this->calendarMode = calendarMode;
    this->clockMode = clockMode;
    this->pomodoroMode = pomodoroMode;
    this->todoMode = todoMode;
}

// =====================================================
// Begin
// =====================================================

void WebCommandManager::begin() {
    setupRouting();
}

// =====================================================
// Get Current Expression State
// =====================================================

void WebCommandManager::updateBaseExpression() {

    expressions->getExpressionStates(
        *currentExpression,
        *baseLeft,
        *baseRight
    );
}

// =====================================================
// Animate Eyes + Keep Current Mouth
// =====================================================

void WebCommandManager::animateEyeMovement(
    EyeState targetLeft,
    EyeState targetRight,
    unsigned long duration
) {

    MouthState currentMouth =
        eyes->getMouth();

    animation->moveTo(
        targetLeft,
        targetRight,
        currentMouth,
        duration
    );

    while (animation->isAnimating()) {

        animation->update();

        delay(16);
    }
}

// =====================================================
// Return To Current Expression
// =====================================================

void WebCommandManager::returnToExpression() {

    animateEyeMovement(
        *baseLeft,
        *baseRight,
        300
    );
}

// =====================================================
// Look Left
// =====================================================

void WebCommandManager::lookLeft() {

    EyeState targetLeft = *baseLeft;
    EyeState targetRight = *baseRight;

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

// =====================================================
// Look Right
// =====================================================

void WebCommandManager::lookRight() {

    EyeState targetLeft = *baseLeft;
    EyeState targetRight = *baseRight;

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

// =====================================================
// Look Up
// =====================================================

void WebCommandManager::lookUp() {

    EyeState targetLeft = *baseLeft;
    EyeState targetRight = *baseRight;

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

// =====================================================
// Look Down
// =====================================================

void WebCommandManager::lookDown() {

    EyeState targetLeft = *baseLeft;
    EyeState targetRight = *baseRight;

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

void WebCommandManager::blink() {

    EyeState closedLeft =
        eyes->getLeftEye();

    EyeState closedRight =
        eyes->getRightEye();

    closedLeft.height = 4;
    closedRight.height = 4;

    closedLeft.pupilSize = 0;
    closedRight.pupilSize = 0;

    MouthState currentMouth =
        eyes->getMouth();

    animation->moveTo(
        closedLeft,
        closedRight,
        currentMouth,
        90
    );

    while (animation->isAnimating()) {

        animation->update();

        delay(16);
    }

    delay(60);

    animation->moveTo(
        *baseLeft,
        *baseRight,
        currentMouth,
        110
    );

    while (animation->isAnimating()) {

        animation->update();

        delay(16);
    }
}

// =====================================================
// Show Expression
// =====================================================

void WebCommandManager::showExpression(
    Expression expression,
    unsigned long duration
) {

    EyeState targetLeft;
    EyeState targetRight;

    expressions->getExpressionStates(
        expression,
        targetLeft,
        targetRight
    );

    MouthState targetMouth =
        expressions->getExpressionMouth(
            expression
        );

    animation->moveTo(
        targetLeft,
        targetRight,
        targetMouth,
        duration
    );

    while (animation->isAnimating()) {

        animation->update();

        delay(16);
    }

    *currentExpression = expression;

    *baseLeft = targetLeft;
    *baseRight = targetRight;

    expressions->setExpression(
        *currentExpression
    );

    eyes->draw();
}

// =====================================================
// Draw To-Do Screen
// =====================================================

void WebCommandManager::drawTodoScreen() {

    display->clearDisplay();

    display->setTextColor(
        SSD1306_WHITE
    );

    display->setTextSize(1);

    display->setCursor(0, 0);
    display->println("TODAY'S TASKS");

    display->drawLine(
        0,
        10,
        127,
        10,
        SSD1306_WHITE
    );

    int count =
        taskManager->getTaskCount();

    if (count == 0) {

        display->setCursor(15, 27);
        display->println("No tasks yet!");

        display->setCursor(7, 42);
        display->println("Add from dashboard");

        display->display();

        return;
    }

    int selected =
        taskManager->getSelectedIndex();

    int visibleStart = 0;

    if (selected >= 4) {
        visibleStart = selected - 3;
    }

    for (
        int screenIndex = 0;
        screenIndex < 4;
        screenIndex++
    ) {

        int taskIndex =
            visibleStart + screenIndex;

        if (taskIndex >= count) {
            break;
        }

        Task task =
            taskManager->getTask(
                taskIndex
            );

        int y =
            14 + (screenIndex * 11);

        if (taskIndex == selected) {

            display->setCursor(0, y);
            display->print(">");
        }

        display->setCursor(9, y);

        if (task.completed) {
            display->print("[x]");
        } else {
            display->print("[ ]");
        }

        display->setCursor(29, y);

        String text =
            task.text;

        if (text.length() > 16) {

            text =
                text.substring(0, 16);
        }

        display->print(text);
    }

    display->setCursor(0, 57);

    display->print(selected + 1);
    display->print("/");
    display->print(count);

    display->display();
}

// =====================================================
// To-Do Celebration
// =====================================================

void WebCommandManager::todoCelebration() {

    display->clearDisplay();

    display->setTextColor(
        SSD1306_WHITE
    );

    display->setTextSize(2);

    display->setCursor(40, 4);
    display->println("^_^");

    display->setTextSize(1);

    display->setCursor(29, 29);
    display->println("WELL DONE!");

    display->setCursor(37, 44);
    display->println("GOOD JOB!");

    display->display();

    delay(1500);
}

// =====================================================
// Exit Modes
// =====================================================

void WebCommandManager::exitSpotifyMode() {

    *spotifyMode = false;

    expressions->setExpression(
        *currentExpression
    );

    eyes->setEyes(
        *baseLeft,
        *baseRight
    );

    eyes->draw();
}

void WebCommandManager::exitCalendarMode() {

    *calendarMode = false;

    expressions->setExpression(
        *currentExpression
    );

    eyes->setEyes(
        *baseLeft,
        *baseRight
    );

    eyes->draw();
}

void WebCommandManager::exitClockMode() {

    *clockMode = false;

    expressions->setExpression(
        *currentExpression
    );

    eyes->setEyes(
        *baseLeft,
        *baseRight
    );

    eyes->draw();
}

void WebCommandManager::exitPomodoroMode() {

    *pomodoroMode = false;

    expressions->setExpression(
        *currentExpression
    );

    eyes->setEyes(
        *baseLeft,
        *baseRight
    );

    eyes->draw();
}

void WebCommandManager::exitTodoMode() {

    *todoMode = false;

    expressions->setExpression(
        *currentExpression
    );

    eyes->setEyes(
        *baseLeft,
        *baseRight
    );

    eyes->draw();
}

// =====================================================
// Return To Normal Mode
// =====================================================

void WebCommandManager::returnToNormalMode() {

    *spotifyMode = false;
    *calendarMode = false;
    *clockMode = false;
    *pomodoroMode = false;
    *todoMode = false;

    expressions->setExpression(
        *currentExpression
    );

    eyes->setEyes(
        *baseLeft,
        *baseRight
    );

    eyes->draw();
}

// =====================================================
// Normal Eye Behavior
// =====================================================

void WebCommandManager::updateNormalEyes() {

    static unsigned long lastBlink = 0;
    static unsigned long lastLook = 0;

    unsigned long now =
        millis();

    // Automatic blink

    if (now - lastBlink > 3500) {

        blink();

        lastBlink =
            millis();
    }

    // Automatic eye movement

    if (now - lastLook > 7000) {

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
}

// =====================================================
// Web API Routing
// =====================================================

void WebCommandManager::setupRouting() {

    // =================================================
    // OPTIONS - Action
    // =================================================

    server->on(
        "/action",
        HTTP_OPTIONS,
        [this]() {

            server->sendHeader(
                "Access-Control-Allow-Origin",
                "*"
            );

            server->sendHeader(
                "Access-Control-Allow-Methods",
                "GET, POST, OPTIONS"
            );

            server->sendHeader(
                "Access-Control-Allow-Headers",
                "Content-Type"
            );

            server->send(204);
        }
    );

    // =================================================
    // OPTIONS - Spotify
    // =================================================

    server->on(
        "/spotify",
        HTTP_OPTIONS,
        [this]() {

            server->sendHeader(
                "Access-Control-Allow-Origin",
                "*"
            );

            server->sendHeader(
                "Access-Control-Allow-Methods",
                "GET, POST, OPTIONS"
            );

            server->sendHeader(
                "Access-Control-Allow-Headers",
                "Content-Type"
            );

            server->send(204);
        }
    );

    // =================================================
    // Spotify GET
    // =================================================

    server->on(
        "/spotify",
        HTTP_GET,
        [this]() {
            spotify->handleGET();
        }
    );

    // =================================================
    // Spotify POST
    // =================================================

    server->on(
        "/spotify",
        HTTP_POST,
        [this]() {
            spotify->handlePOST();
        }
    );

    // =================================================
    // Main Action Endpoint
    // =================================================

    server->on(
        "/action",
        HTTP_GET,
        [this]() {

            server->sendHeader(
                "Access-Control-Allow-Origin",
                "*"
            );

            // =========================================
            // CUSTOM POMODORO TIMER
            // =========================================

            if (
                server->hasArg("focus") &&
                server->hasArg("break")
            ) {

                unsigned long focus =
                    server->arg("focus").toInt();

                unsigned long breakTime =
                    server->arg("break").toInt();

                Serial.println();
                Serial.println(
                    "===== CUSTOM POMODORO ====="
                );

                Serial.print("Focus: ");
                Serial.println(focus);

                Serial.print("Break: ");
                Serial.println(breakTime);

                if (
                    focus == 0 ||
                    breakTime == 0
                ) {

                    server->send(
                        400,
                        "text/plain",
                        "Invalid timer values"
                    );

                    return;
                }

                pomodoro->setTimer(
                    focus,
                    breakTime
                );

                *spotifyMode = false;
                *calendarMode = false;
                *clockMode = false;
                *todoMode = false;
                *pomodoroMode = true;

                pomodoroManager->drawScreen();

                Serial.println(
                    "Custom Pomodoro timer updated."
                );

                Serial.println(
                    "============================"
                );

                server->send(
                    200,
                    "text/plain",
                    "Timer updated"
                );

                return;
            }

            // =========================================
            // Check Command
            // =========================================

            if (!server->hasArg("cmd")) {

                server->send(
                    400,
                    "text/plain",
                    "Error: Missing 'cmd' argument"
                );

                return;
            }

            String cmd =
                server->arg("cmd");

            cmd.toUpperCase();
            cmd.trim();

            Serial.print(
                "API Command Received: "
            );

            Serial.println(cmd);

            // =========================================
            // To-Do Navigation
            // =========================================

            if (
                cmd == "UP" &&
                *todoMode
            ) {

                taskManager->selectPrevious();

                drawTodoScreen();

                server->send(
                    200,
                    "text/plain",
                    "Previous task"
                );

                return;
            }

            if (
                cmd == "DOWN" &&
                *todoMode
            ) {

                taskManager->selectNext();

                drawTodoScreen();

                server->send(
                    200,
                    "text/plain",
                    "Next task"
                );

                return;
            }

            if (
                cmd == "ENTER" &&
                *todoMode
            ) {

                int index =
                    taskManager->getSelectedIndex();

                if (
                    index >= 0 &&
                    index < taskManager->getTaskCount()
                ) {

                    Task task =
                        taskManager->getTask(index);

                    bool wasCompleted =
                        task.completed;

                    bool success =
                        taskManager->toggleTask(
                            task.id
                        );

                    if (success) {

                        taskManager->fetchTasks();

                        if (!wasCompleted) {

                            todoCelebration();
                        }

                        drawTodoScreen();
                    }
                }

                server->send(
                    200,
                    "text/plain",
                    "Task toggled"
                );

                return;
            }

            // =========================================
            // To-Do Mode
            // =========================================

            else if (cmd == "8") {

                if (*todoMode) {

                    exitTodoMode();

                    server->send(
                        200,
                        "text/plain",
                        "Todo mode exited"
                    );

                    return;
                }

                *spotifyMode = false;
                *calendarMode = false;
                *clockMode = false;
                *pomodoroMode = false;

                *todoMode = true;

                Serial.println(
                    "Fetching today's tasks..."
                );

                taskManager->fetchTasks();

                drawTodoScreen();

                server->send(
                    200,
                    "text/plain",
                    "Todo mode"
                );

                return;
            }

            // =========================================
            // Expressions
            // =========================================

            else if (cmd == "HAPPY") {

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

            else if (cmd == "LEFT") {

                if (
                    !*spotifyMode &&
                    !*calendarMode &&
                    !*clockMode &&
                    !*pomodoroMode &&
                    !*todoMode
                ) {

                    lookLeft();
                }
            }

            else if (cmd == "RIGHT") {

                if (
                    !*spotifyMode &&
                    !*calendarMode &&
                    !*clockMode &&
                    !*pomodoroMode &&
                    !*todoMode
                ) {

                    lookRight();
                }
            }

            else if (cmd == "LOOK_LEFT") {

                if (
                    !*spotifyMode &&
                    !*calendarMode &&
                    !*clockMode &&
                    !*pomodoroMode &&
                    !*todoMode
                ) {

                    lookLeft();
                }
            }

            else if (cmd == "LOOK_RIGHT") {

                if (
                    !*spotifyMode &&
                    !*calendarMode &&
                    !*clockMode &&
                    !*pomodoroMode &&
                    !*todoMode
                ) {

                    lookRight();
                }
            }

            else if (cmd == "LOOK_UP") {

                if (
                    !*spotifyMode &&
                    !*calendarMode &&
                    !*clockMode &&
                    !*pomodoroMode &&
                    !*todoMode
                ) {

                    lookUp();
                }
            }

            else if (cmd == "LOOK_DOWN") {

                if (
                    !*spotifyMode &&
                    !*calendarMode &&
                    !*clockMode &&
                    !*pomodoroMode &&
                    !*todoMode
                ) {

                    lookDown();
                }
            }

            else if (
                cmd == "BLINK" ||
                cmd == "WINK"
            ) {

                if (
                    !*spotifyMode &&
                    !*calendarMode &&
                    !*clockMode &&
                    !*pomodoroMode &&
                    !*todoMode
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

                *spotifyMode = false;
                *clockMode = false;
                *pomodoroMode = false;
                *todoMode = false;

                *calendarMode = true;

                calendarManager->drawScreen();
            }

            // =========================================
            // Clock
            // =========================================

            else if (
                cmd == "TIME" ||
                cmd == "CLOCK" ||
                cmd == "2"
            ) {

                *spotifyMode = false;
                *calendarMode = false;
                *pomodoroMode = false;
                *todoMode = false;

                *clockMode = true;

                timeManager->drawScreen();
            }

            // =========================================
            // Spotify
            // =========================================

            else if (
                cmd == "SPOTIFY" ||
                cmd == "3"
            ) {

                *calendarMode = false;
                *clockMode = false;
                *pomodoroMode = false;
                *todoMode = false;

                *spotifyMode = true;

                spotify->drawScreen();
            }

            // =========================================
            // Pomodoro Screen
            // =========================================

            else if (
                cmd == "POMODORO" ||
                cmd == "6"
            ) {

                *spotifyMode = false;
                *calendarMode = false;
                *clockMode = false;
                *todoMode = false;

                *pomodoroMode = true;

                pomodoroManager->drawScreen();
            }

            // =========================================
            // Pomodoro Start / Pause
            // =========================================

            else if (
                cmd == "POMODORO_START" ||
                cmd == "4"
            ) {

                *spotifyMode = false;
                *calendarMode = false;
                *clockMode = false;
                *todoMode = false;

                *pomodoroMode = true;

                pomodoro->startPause();

                pomodoroManager->drawScreen();
            }

            // =========================================
            // Pomodoro Reset
            // =========================================

            else if (
                cmd == "POMODORO_RESET" ||
                cmd == "5"
            ) {

                *spotifyMode = false;
                *calendarMode = false;
                *clockMode = false;
                *todoMode = false;

                *pomodoroMode = true;

                pomodoro->reset();

                pomodoroManager->drawScreen();
            }

            // =========================================
            // Normal Eyes
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
            // Exit
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

                server->send(
                    400,
                    "text/plain",
                    "Error: Unknown command '" +
                    cmd +
                    "'"
                );

                return;
            }

            // =========================================
            // Success
            // =========================================

            server->send(
                200,
                "text/plain",
                "Command Executed: " +
                cmd
            );
        }
    );
}