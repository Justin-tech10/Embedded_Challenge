#include <Arduino.h>

// Setup ADC1, (ADC2, ADC3 are used internally can;t be configured)
// INT1, INT2 accesible
// Serial Communication SDA, SDO, MISO, MOSI accesible


#include <Adafruit_CircuitPlayground.h>

float X, Y, Z;

void setup() {
  Serial.begin(9600);
  CircuitPlayground.begin();
}

void loop() {
  // 'X motion'
  X = CircuitPlayground.motionX();
  // Y Motion
  Y = CircuitPlayground.motionY();
  // Z Motion
  Z = CircuitPlayground.motionZ();

  float magnitude = sqrt((X * X) + (Y * Y) + (Z * Z));

  // Between 8.6 and 12 magnitude is like a tremor
  // 15 and above seems to be excessive arm motion
  // Regular arm swing between 12 and 15


  Serial.print("X: ");
  Serial.print(X);
  Serial.print("  Y: ");
  Serial.print(Y);
  Serial.print("  Z: ");
  Serial.println(Z);
  Serial.print("Magnitude: ");
  Serial.println(magnitude);

  delay(1000);
}
