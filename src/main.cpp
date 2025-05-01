#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define NUM_PIXELS     10
#define NEOPIXEL_PIN   17
#define SPEAKER_PIN    5

Adafruit_NeoPixel pixels(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

uint8_t leftPixels[]  = {5, 6, 7, 8, 9};
uint8_t rightPixels[] = {0, 1, 2, 3, 4};

bool leftActive = false;
bool rightActive = false;

unsigned long previousBeepMillis = 0;
bool toneOn = false;
unsigned long beepInterval = 200;
int toneFrequency = 0;
int currentBrightness = 0;

void setup() {
  pixels.begin();
  pixels.clear();
  pixels.setBrightness(35);
  pixels.show();

  Serial.begin(9600);
  Serial.println("Enter 3.0–5.0 (left red) or 5.0–7.0 (right green):");
}

int mapFloat(float x, float in_min, float in_max, int out_min, int out_max);

void loop() {
  // Serial input handling
  if (Serial.available()) {
    float value = Serial.parseFloat();

    if (value >= 3.0 && value <= 5.0) {
      currentBrightness = mapFloat(value, 3.0, 5.0, 50, 200);
      toneFrequency = mapFloat(currentBrightness, 50, 200, 300, 800);

      // Turn off right side
      for (uint8_t i = 0; i < sizeof(rightPixels); i++) {
        pixels.setPixelColor(rightPixels[i], 0);
      }

      // Turn on left side red
      for (uint8_t i = 0; i < sizeof(leftPixels); i++) {
        pixels.setPixelColor(leftPixels[i], pixels.Color(currentBrightness, 0, 0));
      }

      leftActive = true;
      rightActive = false;
      pixels.show();

      Serial.print("Left RED - Brightness: ");
      Serial.print(currentBrightness);
      Serial.print(" | Tone: ");
      Serial.println(toneFrequency);

    } else if (value > 5.0 && value <= 7.0) {
      currentBrightness = mapFloat(value, 5.0, 7.0, 50, 200);
      toneFrequency = mapFloat(currentBrightness, 50, 200, 300, 800);

      // Turn off left side
      for (uint8_t i = 0; i < sizeof(leftPixels); i++) {
        pixels.setPixelColor(leftPixels[i], 0);
      }

      // Turn on right side green
      for (uint8_t i = 0; i < sizeof(rightPixels); i++) {
        pixels.setPixelColor(rightPixels[i], pixels.Color(0, currentBrightness, 0));
      }

      leftActive = false;
      rightActive = true;
      pixels.show();

      Serial.print("Right GREEN - Brightness: ");
      Serial.print(currentBrightness);
      Serial.print(" | Tone: ");
      Serial.println(toneFrequency);
    } else {
      Serial.println("Invalid input. Use 3.0–5.0 (left) or 5.0–7.0 (right).");
    }
  }

  // Handle alternating tone
  if ((leftActive || rightActive) && millis() - previousBeepMillis >= beepInterval) {
    previousBeepMillis = millis();

    if (toneOn) {
      noTone(SPEAKER_PIN);
    } else {
      tone(SPEAKER_PIN, toneFrequency);
    }

    toneOn = !toneOn;
  }
}

int mapFloat(float x, float in_min, float in_max, int out_min, int out_max) {
  return int((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}




