/*
 * init.ino
 *
 * Initialization routines
 * 
*/
#include "debug.h"
#include "eprom.h"
#include "init.h"
#include "sensor.h"
#include <Adafruit_Si7021.h>

//
// Initialize Hardware Settings
//
bool initHardware() {
  DEBUG_PRINT("Function-Begin :: initHardware");
  // put any hardware initialization code here:

  //
  // Initialize serial output
  //
  DEBUG_PRINT("Setting baudrate . . ." );
  Serial.begin(BAUDRATE);

  // wait for serial port to open
  DEBUG_PRINT("Waiting for serial port to intialize . . .");
  while (!Serial) {
    delay(10);
  }
  DEBUG_PRINT("Serial port initialized.");

  //
  // Set the EEPROM size if using ESP8266
  //
  DEBUG_PRINT("Using ESP8266 - initializing EEPROM . . .");
  initEEPROM();
  
  //
  // Check for sensor
  //
  DEBUG_PRINT( "Checking for Si7021 sensor . . ." );
  if (!initSensor( &sensor )) {
    Serial.println("Did not find Si7021 sensor!");
    // Hang if not found
    while (true);
  } else {
    SensorFound = true;
  }
  
  DEBUG_PRINT("Sensor found.");

  DEBUG_PRINT("Function-End :: initHardware");
}
