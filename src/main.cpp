#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

unsigned long lastBlinkTime = 0;
unsigned long blinkInterval = 3000;

void drawEyes(bool blink) {

  display.clearDisplay();

  if (blink) {
    // Closed eyes
    display.fillRect(25, 31, 30, 4, SSD1306_WHITE);
    display.fillRect(73, 31, 30, 4, SSD1306_WHITE);
  }
  else {
    // Open eyes
    display.fillRoundRect(20, 18, 38, 30, 8, SSD1306_WHITE);
    display.fillRoundRect(70, 18, 38, 30, 8, SSD1306_WHITE);

    // Pupils
    display.fillCircle(39, 33, 7, SSD1306_BLACK);
    display.fillCircle(89, 33, 7, SSD1306_BLACK);
  }

  display.display();
}

void setup() {

  Serial.begin(115200);

  Wire.begin(21, 22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("OLED not found!");
    while (true);
  }

  drawEyes(false);
}

void loop() {

  unsigned long now = millis();

  if (now - lastBlinkTime > blinkInterval) {

    // Close eyes
    drawEyes(true);
    delay(150);

    // Open eyes
    drawEyes(false);

    lastBlinkTime = now;
  }

  delay(50);
}