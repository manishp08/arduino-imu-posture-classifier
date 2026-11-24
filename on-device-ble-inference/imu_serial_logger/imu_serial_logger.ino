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
  float ax, ay, az, gx, gy, gz, mx, my, mz;
  if (IMU.readAcceleration(ax, ay, az) & IMU.readGyroscope(gx, gy, gz) & IMU.readMagneticField(mx, my, mz)) 
  {
    // Print accelerometer data to the serial monitor
    Serial.print("Ax: ");
    Serial.print(ax);
    Serial.print("\tAy: ");
    Serial.print(ay);
    Serial.print("\tAz: ");
    Serial.print(az);
    //Print gyroscope data to the serial monitor
    Serial.print("\tGx: ");
    Serial.print(gx);
    Serial.print("\tGy: ");
    Serial.print(gy);
    Serial.print("\tGz: ");
    Serial.print(gz);
    //print magneticfield data to the serial monitor
    Serial.print("\tMx: ");
    Serial.print(mx);
    Serial.print("\tMy: ");
    Serial.print(my);
    Serial.print("\tMz: ");
    Serial.println(mz);
  }
  /*
  if (IMU.readGyroscope(gx, gy, gz))
  {
    //Print gyroscope data to the serial monitor
    Serial.print("Gx: ");
    Serial.print(gx);
    Serial.print("\tGy: ");
    Serial.print(gy);
    Serial.print("\tGz: ");
    Serial.println(gz);
  }
  if (IMU.readMagneticField(mx, my, mz))
  {
    //print magneticfield data to the serial monitor
    Serial.print("Mx: ");
    Serial.print(mx);
    Serial.print("\tMy: ");
    Serial.print(my);
    Serial.print("\tMz: ");
    Serial.println(mz);
  }
*/
  delay(100); // Adjust the delay as needed for your application
}