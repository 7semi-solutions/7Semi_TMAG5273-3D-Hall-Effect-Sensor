/**
 * Basic Read Example
 *
 * Sensor
 * - Texas Instruments TMAG5273
 * - 3-Axis Hall Effect Magnetic Sensor
 *
 * Hardware Connection
 *
 * TMAG5273      Microcontroller
 * --------------------------------
 * VCC    ->     3.3V
 * GND    ->     GND
 * SDA    ->     SDA
 * SCL    ->     SCL
 *
 * Optional (not required)
 * INT    ->     Interrupt Pin
 *
 * Default I2C Address
 * - 0x35
 *
 * Serial Output
 * - X magnetic field
 * - Y magnetic field
 * - Z magnetic field
 */

#include <7Semi_TMAG5273.h>

TMAG5273_7Semi sensor;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (!sensor.begin(0x35)) {
    Serial.println("TMAG5273 not detected");
    while (1)
      ;
  }

  Serial.println("TMAG5273 initialized");
}

void loop() {
  float x, y, z;
  float temperature;

    /**
    * Read magnetic field values
    */
  if (sensor.readMag(x, y, z)) {
    Serial.print("X: ");
    Serial.print(x);
    Serial.print(" mT  ");

    Serial.print("Y: ");
    Serial.print(y);
    Serial.print(" mT  ");

    Serial.print("Z: ");
    Serial.print(z);
    Serial.println(" mT");
  }

   /**
   * Read temperature
   */
  if (sensor.readTemperatureC(temperature)) {
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" C");
  }
  delay(500);
}