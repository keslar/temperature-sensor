/*
 * sensor.ino
 *
 * sensor routines 
 * 
*/
#include "debug.h"
#include "sensor.h"
#include <Adafruit_Si7021.h>

// Initialize sensor
bool initSensor(Adafruit_Si7021* sensor ) {
  return sensor.begin();
}

// Read the sensor and send info to server
void readSensor() {
  DEBUG_PRINT("Function-Begin :: sensorThread");
  sensorTemp = sensor.readTemperature();
  sensorHumidity = sensor.readTemperature();

  Serial.print("Humidity:    "); Serial.print(sensor.readHumidity(), 2);
  Serial.print("\tTemperature: "); Serial.println(sensor.readTemperature(), 2);

  // Send the sensor reading to server if we are supposed to.

  DEBUG_PRINT("Function-End :: sensorThread");
}

