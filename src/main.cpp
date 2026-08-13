#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// simple eye state
int eyeHeight = 20;
bool blinking = false;
unsigned long lastBlinkTime = 0;
unsigned long blinkInterval = 3000; // blink every ~3s

void drawEyes(int height) {
  display.clearDisplay();

  int eyeWidth = 18;
  int leftX = 34;
  int rightX = 76;
  int centerY = 28;

  // two rounded-rect eyes, height changes to simulate blinking
  display.fillRoundRect(leftX - eyeWidth/2, centerY - height/2, eyeWidth, height, 4, SSD1306_WHITE);
  display.fillRoundRect(rightX - eyeWidth/2, centerY - height/2, eyeWidth, height, 4, SSD1306_WHITE);

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(30, 54);
  display.print("HELLO");

  display.display();
}

void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true); // halt if OLED not found
  }

  display.clearDisplay();
  drawEyes(eyeHeight);
}

void loop() {
  unsigned long now = millis();

  // trigger a blink every few seconds
  if (!blinking && now - lastBlinkTime > blinkInterval) {
    blinking = true;
    lastBlinkTime = now;
  }

  if (blinking) {
    // quick blink animation: shrink eyes then grow back
    for (int h = 20; h >= 2; h -= 4) {
      drawEyes(h);
      delay(20);
    }
    for (int h = 2; h <= 20; h += 4) {
      drawEyes(h);
      delay(20);
    }
    blinking = false;
  }

  delay(50);
}