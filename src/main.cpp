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

    animateEyeMovement(targetLeft, targetRight, 300);
    delay(300);
    returnToExpression();
}

void lookRight() {
    EyeState targetLeft = baseLeft;
    EyeState targetRight = baseRight;

    targetLeft.x += 10;
    targetRight.x += 10;

    animateEyeMovement(targetLeft, targetRight, 300);
    delay(300);
    returnToExpression();
}

void lookUp() {
    EyeState targetLeft = baseLeft;
    EyeState targetRight = baseRight;

    targetLeft.y -= 7;
    targetRight.y -= 7;

    animateEyeMovement(targetLeft, targetRight, 300);
    delay(300);
    returnToExpression();
}

void lookDown() {
    EyeState targetLeft = baseLeft;
    EyeState targetRight = baseRight;

    targetLeft.y += 7;
    targetRight.y += 7;

    animateEyeMovement(targetLeft, targetRight, 300);
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
    animation.moveTo(closedLeft, closedRight, currentMouth, 90);
    while (animation.isAnimating()) {
        animation.update();
        delay(16);
    }

    delay(60);

    // Open
    animation.moveTo(baseLeft, baseRight, currentMouth, 110);
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

    MouthState targetMouth = expressions.getExpressionMouth(expression);

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

    expressions.setExpression(currentExpression);
    eyes.draw();
}

// =====================================================
// Web API Routing (UPDATED WITH PROPER CORS)
// =====================================================

void setupRouting() {
    // Handle CORS preflight options check
    server.on("/action", HTTP_OPTIONS, []() {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
        server.send(204); 
    });

    // Handle GET actions
    server.on("/action", HTTP_GET, []() {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        
        if (server.hasArg("cmd")) {
            String cmd = server.arg("cmd");
            cmd.toUpperCase(); // Ignores lowercase vs uppercase errors
            cmd.trim();        // Removes accidental whitespace
            
            Serial.print("API Command Received: ");
            Serial.println(cmd);
            
            // Route Expressions
            if (cmd == "HAPPY") showExpression(EXPRESSION_HAPPY, 500);
            else if (cmd == "SAD") showExpression(EXPRESSION_SAD, 500);
            else if (cmd == "ANGRY") showExpression(EXPRESSION_ANGRY, 500);
            else if (cmd == "SURPRISED" || cmd == "SHOCK") showExpression(EXPRESSION_SURPRISED, 500);
            else if (cmd == "SLEEPY") showExpression(EXPRESSION_SLEEPY, 500);
            else if (cmd == "NEUTRAL" || cmd == "RESET") showExpression(EXPRESSION_NEUTRAL, 500);
            
            // Route Movement & Actions
            else if (cmd == "LOOK_LEFT" || cmd == "LEFT") lookLeft();
            else if (cmd == "LOOK_RIGHT" || cmd == "RIGHT") lookRight();
            else if (cmd == "LOOK_UP" || cmd == "UP") lookUp();
            else if (cmd == "LOOK_DOWN" || cmd == "DOWN") lookDown();
            else if (cmd == "BLINK" || cmd == "WINK") blink();
            else {
                server.send(400, "text/plain", "Error: Unknown command '" + cmd + "'");
                return;
            }
            
            server.send(200, "text/plain", "Command Executed: " + cmd);
        } else {
            server.send(400, "text/plain", "Error: Missing 'cmd' argument");
        }
    });
}
// =====================================================
// Setup
// =====================================================

void setup() {
    Serial.begin(115200);

    // ESP32 I2C
    Wire.begin(21, 22);

    // OLED
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println("OLED not found!");
        while (true) { delay(1000); }
    }

    Serial.println("MoMo Eye Engine V2 Started");

    // Wi-Fi Setup
    Serial.print("Connecting to Wi-Fi");
    WiFi.mode(WIFI_STA); 
    WiFi.disconnect();
    delay(100);
    WiFi.begin(ssid, password);

    int timeout = 0;
    while (WiFi.status() != WL_CONNECTED && timeout < 30) {
        delay(1000);
        timeout++;
        Serial.print("Status code: ");
        Serial.println(WiFi.status());
    }

    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("\n--- WiFi Connection Failed ---");
    } else {
        Serial.println("\nConnected successfully!");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
        WiFi.setSleep(false); // Keeps Ping Time Low!
    }
    
    // Start Web Server
    setupRouting();
    server.begin();

    // Start neutral expression
    currentExpression = EXPRESSION_NEUTRAL;
    expressions.setExpression(currentExpression);
    updateBaseExpression();
    eyes.draw();
}

// =====================================================
// Main Loop
// =====================================================

void loop() {
    // 1. Listen for incoming web UI commands
    server.handleClient();

    static unsigned long lastBlink = 0;
    static unsigned long lastLook = 0;
    
    unsigned long now = millis();

    // =================================================
    // Idle Blink
    // =================================================
    if (now - lastBlink > 3500) {
        blink();
        lastBlink = millis();
    }

    // =================================================
    // Idle Eye Movement
    // =================================================
    if (now - lastLook > 7000) {
        int movement = random(0, 4);

        if (movement == 0) lookLeft();
        else if (movement == 1) lookRight();
        else if (movement == 2) lookUp();
        else lookDown();

        lastLook = millis();
    }

    delay(16);
}