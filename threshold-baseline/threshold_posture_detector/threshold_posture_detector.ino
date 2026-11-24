#include <Arduino_LSM9DS1.h>
float threshold = 0.1;

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Started");

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  pinMode(LED_BUILTIN, OUTPUT); // Set the onboard LED as an output
  digitalWrite(LED_BUILTIN, LOW); // Initially, turn the LED off
}

void loop() {
  float accelX, accelY, accelZ;
  IMU.readAcceleration(accelX, accelY, accelZ);

  // Check posture based on accelerometer data
  if (isSupine(accelX, accelY, accelZ)) {
    displayPosture("Supine");
    blinkLED(1); // Blink the LED once
  } else if (isProne(accelX, accelY, accelZ)) {
    displayPosture("Prone");
    blinkLED(2); // Blink the LED twice
  } else if (isSide(accelX, accelY, accelZ)) {
    displayPosture("Side");
    blinkLED(3); // Blink the LED three times
  } else {
    displayPosture("Unknown");
    digitalWrite(LED_BUILTIN, LOW); // Keep the LED off
  }

  delay(1000); // Delay for stability and readability (1 second)
}

bool isSupine(float x, float y, float z) {
  return (z > x && z > y && z >= 0.8 && z <= 1.2);
}

bool isProne(float x, float y, float z) {
  return (z < x && z < y && z >= -1.2 && z <= -0.8);
}

bool isSide(float x, float y, float z) {
  return ((y > x && y > z && y >= 0.8 && y <= 1.2) || (y < x && y < z && y >= -1.2 && y <= -0.8));
}

void displayPosture(const char* posture) {
  Serial.print("Detected Posture: ");
  Serial.println(posture);
}

void blinkLED(int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(LED_BUILTIN, HIGH); // Turn the LED on
    delay(200); // Delay for 200 ms (.2 a second)
    digitalWrite(LED_BUILTIN, LOW); // Turn the LED off
    delay(200); // Delay for 200 ms (.2 a second)
  }
}
