#include <TFT_eSPI.h> // Include the library
#include <SPI.h>
#include "esp_sleep.h"


#include <Pangodream_18650_CL.h> 

#define ADC_PIN 34
#define CONV_FACTOR 1.8
#define READS 20
Pangodream_18650_CL BL(ADC_PIN, CONV_FACTOR, READS);

#define BUTTON_PIN_BITMASK 0x800000000 // 2^35 in hex

void IRAM_ATTR goToSleep() {
  esp_deep_sleep_start();
}

// actual setup 

TFT_eSPI tft = TFT_eSPI(); // Create display object
TFT_eSprite img = TFT_eSprite(&tft);
TFT_eSprite txtSprite = TFT_eSprite(&tft); 
TFT_eSprite bottomTxtSprite = TFT_eSprite(&tft); 
TFT_eSprite finalTxtSprite = TFT_eSprite(&tft); 


// Constant flower properties
const int centerX = 120; // Center of the flower
const int centerY = 70;
const int petalLength = 30; // Length of the petal

void setup() {
  tft.init();
  tft.setRotation(1);
  // attachInterrupt(0, goToSleep, CHANGE);
  tft.fillScreen(TFT_BLACK);
  // set screen Back Light brightness
  pinMode(TFT_BL, OUTPUT);
  ledcSetup(0, 5000, 8); // 0-15, 5000, 8
  ledcAttachPin(TFT_BL, 0); // TFT_BL, 0 - 15
  ledcWrite(0, 125); // 0-15, 0-255 (with 8 bit resolution); 0=totally dark;255=totally shiny

  img.createSprite(240,135);
  

  txtSprite.createSprite(tft.width()/2, 30);
  bottomTxtSprite.createSprite(tft.width()/3, 30);
  finalTxtSprite.createSprite(240, 135);
}
String topTxtOptions[] = {"He loves", "She loves", "They love", "I love"};
int numMessages = sizeof(topTxtOptions) / sizeof(topTxtOptions[0]);
int loopCounter = 0;

void loop() {
  loopCounter++;
  if(loopCounter == 5) {
    esp_sleep_enable_timer_wakeup(5 * 1000000); // Wake up after 5 seconds
    esp_deep_sleep_start();
  }

  randomSeed(analogRead(0));
  int numPetals = random(6,12); 
  int petalWidth = max(10, 140 / numPetals);

  drawFlower(centerX, centerY, petalLength, petalWidth, numPetals, TFT_RED);
  delay(2000);

  int count = 1; 
  String topTxt = topTxtOptions[random(numMessages)];
  String bottomTxt; 

  for (int i = numPetals; i > 0; i--) {
    removePetal(centerX, centerY, numPetals, petalLength, petalWidth, i, TFT_BLACK);

    txtSprite.fillSprite(TFT_BLACK); // Clear the sprite by filling it with black
    txtSprite.setTextColor(TFT_WHITE, TFT_BLACK); // Set text color with background
    txtSprite.setTextSize(2);

    bottomTxtSprite.fillSprite(TFT_BLACK); // Clear the sprite by filling it with black
    bottomTxtSprite.setTextColor(TFT_WHITE, TFT_BLACK); // Set text color with background
    bottomTxtSprite.setTextSize(2);

    if (count % 2 == 0) {
      bottomTxt = "me not";
      txtSprite.setCursor(120-txtSprite.textWidth(topTxt)-5, 0); 
      bottomTxtSprite.setCursor(80-txtSprite.textWidth(bottomTxt)-5, 0);

      txtSprite.println(topTxt);
      bottomTxtSprite.println(bottomTxt);

      txtSprite.pushSprite(120, 0);
      bottomTxtSprite.pushSprite(160,30);
    } else {
      bottomTxt = "me";
      txtSprite.setCursor(5, 0); 
      bottomTxtSprite.setCursor(5, 0);

      txtSprite.println(topTxt);
      bottomTxtSprite.println(bottomTxt);

      txtSprite.pushSprite(0, 0);
      bottomTxtSprite.pushSprite(0,30);
    }
    count += 1; 
    delay(2000);
  }

  finalTxtSprite.fillSprite(TFT_BLACK); // Clear the sprite by filling it with black
  finalTxtSprite.setTextColor(TFT_WHITE, TFT_BLACK); // Set text color with background
  finalTxtSprite.setTextSize(4);
  finalTxtSprite.setCursor((finalTxtSprite.width() - finalTxtSprite.textWidth(topTxt))/2,20);
  finalTxtSprite.println(topTxt);
  finalTxtSprite.setCursor((finalTxtSprite.width() - finalTxtSprite.textWidth(bottomTxt))/2,70);
  finalTxtSprite.println(bottomTxt);
  finalTxtSprite.pushSprite(0,0);
  delay(3000);
}

void drawFlower(int x, int y, int petalLen, int petalWid, int petals, uint32_t color) {
  for (int i = 0; i < petals; i++) {
    float angle = i * 360.0 / petals;
    drawPetal(x, y, petalLen, petalWid, angle, color);
  }
  // Draw the center of the flower last, so it overlaps any petal edges
  img.fillCircle(x, y, petalWid/2, TFT_YELLOW); // Center of the flower
  img.pushSprite(0,0);

}

void drawPetal(int x, int y, int petalLen, int petalWid, float angle, uint32_t color) {
  float radAngle = radians(angle);
  int ellipseRadius = petalWid / 2;
  for (int radiusOffset = 0; radiusOffset < petalLen; radiusOffset += ellipseRadius) {
    int petalX = x + cos(radAngle) * (radiusOffset + ellipseRadius / 2);
    int petalY = y + sin(radAngle) * (radiusOffset + ellipseRadius / 2);
    img.fillCircle(petalX, petalY, ellipseRadius, color);
  }
}


void removePetal(int x, int y, int numPetals, int petalLen, int petalWid, int petalNumber, uint32_t bgColor) {
  float angle = (petalNumber - 1) * 360.0 / numPetals;
  drawPetal(x, y, petalLen, petalWid, angle, bgColor); // Draw over the petal with background color
  // Optionally redraw the center if petals overlap it
  img.fillCircle(x, y, petalWid / 2, TFT_YELLOW); // Center of the flower
  img.pushSprite(0,0);
}
