#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "EyeEngine.h"
#include "AnimationEngine.h"
#include "ExpressionEngine.h"


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

Expression currentExpression =
    EXPRESSION_NEUTRAL;


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
// Look Left
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


// =====================================================
// Look Right
// =====================================================

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


// =====================================================
// Look Up
// =====================================================

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


// =====================================================
// Look Down
// =====================================================

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


    MouthState currentMouth =
        eyes.getMouth();


    // -------------------------------------------------
    // Close
    // -------------------------------------------------

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


    // -------------------------------------------------
    // Open
    // -------------------------------------------------

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


    // -------------------------------------------------
    // Generate target eyes
    // -------------------------------------------------

    expressions.getExpressionStates(
        expression,
        targetLeft,
        targetRight
    );


    // -------------------------------------------------
    // Generate target mouth
    // -------------------------------------------------

    MouthState targetMouth =
        expressions.getExpressionMouth(
            expression
        );


    // -------------------------------------------------
    // Animate eyes + mouth together
    // -------------------------------------------------

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


    // -------------------------------------------------
    // Commit expression
    // -------------------------------------------------

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
// Setup
// =====================================================

void setup() {

    Serial.begin(115200);


    // -------------------------------------------------
    // ESP32 I2C
    // -------------------------------------------------

    Wire.begin(
        21,
        22
    );


    // -------------------------------------------------
    // OLED
    // -------------------------------------------------

    if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        SCREEN_ADDRESS
    )) {

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


    // -------------------------------------------------
    // Start neutral expression
    // -------------------------------------------------

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

    static unsigned long lastBlink = 0;
    static unsigned long lastLook = 0;
    static unsigned long lastExpression = 0;


    unsigned long now =
        millis();


    // =================================================
    // Blink
    // =================================================

    if (now - lastBlink > 3500) {

        blink();

        lastBlink =
            millis();
    }


    // =================================================
    // Eye Movement
    // =================================================

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


    // =================================================
    // Expression Demo
    // =================================================

    if (now - lastExpression > 10000) {

        int expression =
            random(
                EXPRESSION_NEUTRAL,
                EXPRESSION_SLEEPY + 1
            );


        showExpression(
            (Expression)expression,
            500
        );


        lastExpression =
            millis();
    }


    delay(16);
}