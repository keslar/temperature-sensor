/*
 * 
 * Remote Temperature and Humidity Sendor
 * 
 * Initialize EEPROM and test sensor
 * 
 */

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include "Adafruit_Si7021.h"

#define DEBUG

#ifdef DEBUG
 #define DEBUG_PRINT(x)  Serial.println(x)
#else
 #define DEBUG_PRINT(x)
#endif


/* 
 *  Configuration and EEPROM settings
 */
#define BAUDRATE 115200 
#define EEPROM_SIZE 1024
#define SALT "akddjuemv"
#define WEBSERVER_PORT 80

struct Configuration {
  // WiFi Settings
  String  ssid            = "None";
  int     ssidSize        = 32;    // max: 32
  String  pskey             = "wifi password";
  int     pskeySize         = 64;     // max: 64
  // Device Info
  String  name            = "";
  int     nameSize        = 64;
  String  serialNo        = "";
  int     serialNoSize    = 17;
  // Admin password
  String  passwdHash      = "";
  int     passwdHashSize  = 40;
};
Configuration sysconfig;

String defaultPassword = "admin";

// Sensor
bool SensorFound = false;
Adafruit_Si7021 sensor = Adafruit_Si7021();
#define SENSORPOLLINTERVAL 300000
int sensorTemp;
int sensorHumidity;

void setup() {
  DEBUG_PRINT("Function-Begin :: setup");
  // Initialize any hardware
  DEBUG_PRINT("Initializing hardware . . .");
  initHardware();

  // Clear the EEPROM
  DEBUG_PRINT("Clearning EEPROM . . .");
  clearConfiguration();
  

  // Set default values
  sysconfig.ssid = "Sensor-Setup";
  sysconfig.pskey = "";
  sysconfig.name = "undefined";
  sysconfig.serialNo = WiFi.macAddress();
  String salted = strcat(SALT,"admin");
  sysconfig.passwdHash = sha1( salted );
  
  // Write deafult config to EEPROM
  DEBUG_PRINT("Writing default values to EEPROM . . .");
  saveConfiguration();

  DEBUG_PRINT("Function-End :: setup");
}

void loop() {
  DEBUG_PRINT("Function-Begin :: loop");
  // put your main code here, to run repeatedly:
  
  if ( SensorFound ) {
    DEBUG_PRINT( "Reading sensor values . . .");
    readSensors();
    Serial.print("Humidity:    "); Serial.print(sensor.readHumidity(), 2);
    Serial.print("\tTemperature: "); Serial.println(sensor.readTemperature(), 2);
    delay(10000);
  } else {
    Serial.println( "Sensor not found." );
    while (true);
  }
  DEBUG_PRINT("Function-End :: loop");
}

//
// Initialize Hardware Settings
//
void initHardware() {
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
  DEBUG_PRINT("Using ESP8266 - initializing EEPROM to . . .");
  EEPROM.begin( EEPROM_SIZE );

  //
  // Check for sensor
  //
  DEBUG_PRINT( "Checking for Si7021 sensor . . ." ); 
  if (!sensor.begin()) {
    Serial.println("Did not find Si7021 sensor!");
    // Hang if not found
    while (true);
  } else {
    SensorFound = true;
  }
  DEBUG_PRINT("Sensor found.");

  DEBUG_PRINT("Function-End :: initHardware");
}

void clearConfiguration() {
  DEBUG_PRINT("Function-Begin :: clearConfiguration");
  // Clear the configuraiton from the EEPROM

  // Write NULLs into the EEPROM
  Serial.println( "Clearing EEPROM . . ." );
  for( int i = 0; i < EEPROM_SIZE; i++ ) {
    Serial.print( ".");
    EEPROM.write( i, 0x0 );
  }
  Serial.println("#");

  EEPROM.commit();
  
  DEBUG_PRINT("Function-End :: clearConfiguration");
}

void saveConfiguration() {
  DEBUG_PRINT("Function-Begin :: saveConfiguration");
  // Save the configuration to EEPROM

  int memlocation = 0;
  
  // Wireless config
  DEBUG_PRINT( "Writing SSID . . ." );
  writeEEPROM( memlocation, sysconfig.ssidSize,sysconfig.ssid );
  memlocation = memlocation + 1 + sysconfig.ssidSize;
  DEBUG_PRINT( "Writing PSK . . ." );
  writeEEPROM( memlocation,sysconfig.pskeySize,sysconfig.pskey );
  memlocation = memlocation + 1 + sysconfig.pskeySize;

  // Device Info
  DEBUG_PRINT( "Writing sensor name . . ." );
  writeEEPROM( memlocation, sysconfig.nameSize, sysconfig.name );
  memlocation = memlocation + 1 + sysconfig.nameSize;
  DEBUG_PRINT( "Writing serial number . . ." );
  writeEEPROM( memlocation, sysconfig.serialNoSize, sysconfig.serialNo );
  memlocation = memlocation + 1 + sysconfig.serialNoSize;

  // Device Admin password hash
  DEBUG_PRINT( "Writing admin password hash . . ." );
  writeEEPROM( memlocation, sysconfig.passwdHashSize, sysconfig.passwdHash );
  DEBUG_PRINT("Function-End :: saveConfiguration");
}

String readEEPROM (int start, int size ) {
  // Read a string from the EEPROM
  String  retString;
  int     memcount = start;
  char    c;
  
  for( int i = memcount; i < (memcount+size); i++ ) {
     c = EEPROM.read( i );
     if ( c != 0 ) {
      retString += (c);
     } else {
      break;
     }
  }

  EEPROM.commit();

  return retString;
}

void writeEEPROM( int start, int size, String value ) {
  DEBUG_PRINT("Function-Begin :: writeEEPROM");
  // Write a value into the EEPROM
  DEBUG_PRINT( start );
  DEBUG_PRINT( size );
  DEBUG_PRINT( value );
  for (int i=0; i<(value.length()<size?value.length():size); i++) {
    EEPROM.write(start+i,value.charAt(i));
  }
  
  EEPROM.write( start + (value.length()<size?value.length():size), 0 );

  EEPROM.commit();
  
  DEBUG_PRINT("Function-End :: writeEEPROM");
}


void readSensors() {
  DEBUG_PRINT("Function-Begin :: readSensors");
  DEBUG_PRINT( "Reading temperature sensor . . ." );
  sensorTemp = sensor.readTemperature();
  DEBUG_PRINT( "Reading humidity sensor . . ." );
  sensorHumidity = sensor.readHumidity();
  DEBUG_PRINT("Function-End :: readSensors");
}



