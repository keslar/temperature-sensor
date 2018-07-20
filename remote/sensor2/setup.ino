/*
 * setup.ino - setup and initialization rroutines
 */

#include "setup.h"

// Initialize hardware components
struct SensorObj initHardware() {
  DEBUG_PRINT( "Function-Begin :: initHardware()" );
  
  struct SensorObj MainSensor;

  // Initialize the serial console port
  Serial.println( "Initializing serial console . . . " );
  Serial.print( "Setting baud rate to " + String(BAUDRATE) + " . " );
  Serial.begin( BAUDRATE );
  while(!Serial) {
    Serial.print(". ");
  }
  Serial.println(".");

  // Set how much of the EEPROM we will use
  initEEPROM( EEPROM_SIZE );

  // Initialize the sensor module
  DEBUG_PRINT( "Setting default sensor values . . ." );
  
  MainSensor.sensorname   = "Main Sensor";
  MainSensor.Si7021       = Adafruit_Si7021();
  MainSensor.present      = false;
  MainSensor.temperature  = 0.0;
  MainSensor.humidity     = 0.0;
  
  DEBUG_PRINT( "Checking for Si7021 sensor . . ." );
  MainSensor.present = MainSensor.Si7021.begin();
  if (!MainSensor.present) {
    Serial.println("Did not find Si7021 sensor!");
    Serial.println("System HALTED !!");
    // Hang if not found
    while (true);
  }

  DEBUG_PRINT( "Function-End :: initHardware()" );
  return MainSensor;
}

