#include <Arduino_LSM9DS1.h>

void setup() {
  Serial.begin(9600);

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
}

void loop() 
{
  // Read accelerometer data
  float ax, ay, az;
  if (IMU.readAcceleration(ax, ay, az)) {
    // Print accelerometer data to the serial monitor
    Serial.print("Ax: ");
    Serial.print(ax);
    Serial.print("\tAy: ");
    Serial.print(ay);
    Serial.print("\tAz: ");
    Serial.println(az);
}
  delay(100); // Adjust the delay as needed for your application
}