#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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
// MoMo Eye Parameters
// =====================================================

// Eye size
int eyeWidth = 38;
int eyeHeight = 30;

// Rounded corner size
int cornerRadius = 8;

// Pupil size
int pupilSize = 7;

// Distance between eyes
int eyeSpacing = 50;

// Overall eye position
int eyeCenterX = 64;
int eyeCenterY = 33;

// Current eye movement
int offsetX = 0;
int offsetY = 0;


// =====================================================
// Draw Eyes
// =====================================================

void drawEyes(bool blink = false) {

    display.clearDisplay();

    int leftX = eyeCenterX - eyeSpacing / 2 + offsetX;
    int rightX = eyeCenterX + eyeSpacing / 2 + offsetX;

    int centerY = eyeCenterY + offsetY;


    // -------------------------------------------------
    // Blink
    // -------------------------------------------------

    if (blink) {

        display.fillRect(
            leftX - eyeWidth / 2,
            centerY - 2,
            eyeWidth,
            4,
            SSD1306_WHITE
        );

        display.fillRect(
            rightX - eyeWidth / 2,
            centerY - 2,
            eyeWidth,
            4,
            SSD1306_WHITE
        );
    }


    // -------------------------------------------------
    // Normal Eyes
    // -------------------------------------------------

    else {

        // Left eye
        display.fillRoundRect(
            leftX - eyeWidth / 2,
            centerY - eyeHeight / 2,
            eyeWidth,
            eyeHeight,
            cornerRadius,
            SSD1306_WHITE
        );

        // Right eye
        display.fillRoundRect(
            rightX - eyeWidth / 2,
            centerY - eyeHeight / 2,
            eyeWidth,
            eyeHeight,
            cornerRadius,
            SSD1306_WHITE
        );


        // Left pupil
        display.fillCircle(
            leftX,
            centerY,
            pupilSize,
            SSD1306_BLACK
        );


        // Right pupil
        display.fillCircle(
            rightX,
            centerY,
            pupilSize,
            SSD1306_BLACK
        );
    }

    display.display();
}


// =====================================================
// Blink
// =====================================================

void blink() {

    drawEyes(true);

    delay(150);

    drawEyes(false);
}


// =====================================================
// Eye Movement
// =====================================================

void lookLeft() {

    offsetX = -12;

    drawEyes();

    delay(700);

    offsetX = 0;

    drawEyes();
}


void lookRight() {

    offsetX = 12;

    drawEyes();

    delay(700);

    offsetX = 0;

    drawEyes();
}


void lookUp() {

    offsetY = -10;

    drawEyes();

    delay(700);

    offsetY = 0;

    drawEyes();
}


void lookDown() {

    offsetY = 10;

    drawEyes();

    delay(700);

    offsetY = 0;

    drawEyes();
}


// =====================================================
// Setup
// =====================================================

void setup() {

    Serial.begin(115200);

    // ESP32 I2C
    Wire.begin(21, 22);

    if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        SCREEN_ADDRESS
    )) {

        Serial.println("OLED not found!");

        while (true) {
            delay(1000);
        }
    }

    Serial.println("MoMo Eye Engine Started");

    drawEyes();
}


// =====================================================
// Main Loop
// =====================================================

void loop() {

    // For now, keep the same natural behavior.

    static unsigned long lastBlink = 0;
    static unsigned long lastLook = 0;

    unsigned long now = millis();


    // Random blink
    if (now - lastBlink > 3000) {

        blink();

        lastBlink = millis();
    }


    // Random eye movement
    if (now - lastLook > 7000) {

        int movement = random(0, 4);

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

        lastLook = millis();
    }

    delay(20);
}