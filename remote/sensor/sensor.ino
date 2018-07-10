/*
 * Remote Temperature and Humidity Sendor
 * 
 * Read the temperature and humidy and submit it to 
 * a web server.
 */

#include <EEPROM.h>

#include <ESP8266WiFi.h>
// #include <TimedAction.h>
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
  String  ssid;
  int     ssidSize        = 32;    // max: 32
  String  pskey;
  int     pskeySize         = 64;     // max: 64
  // Device Info
  String  name;
  int     nameSize        = 64;
  String  serialNo;
  int     serialNoSize    = 17;
  // Admin password
  String  passwdHash;
  int     passwdHashSize  = 40;
};
Configuration sysconfig;



// Sensor
bool SensorFound = false;
Adafruit_Si7021 sensor = Adafruit_Si7021();
#define SENSORPOLLINTERVAL 300000
int sensorTemp;
int sensorHumidity;

// Set times action
//TimedAction = sensorThread( 30000, readSensors );

// Set web server port number to 80
//WiFiServer server(80);

// Variable to store the HTTP request
String header;

//
// Setup Routine
//
void setup() {
  DEBUG_PRINT("Function-Begin :: setup");

  // Initialize any hardware
  DEBUG_PRINT("Initializing hardware . . .");
  initHardware();

  // Read Configuration from EEPROM
  DEBUG_PRINT("Reading configuraiton from EEPROM . . .");
  readConfiguration();
  
  // Setup Wireless Network Connection
//  connectWiFi();

  DEBUG_PRINT("Function-End :: setup");
}


void loop() {
  DEBUG_PRINT("Function-Begin :: loop");
  
  DEBUG_PRINT( "Reading sensor values . . .");
  readSensors();
  Serial.print("Humidity:    "); Serial.print(sensor.readHumidity(), 2);
  Serial.print("\tTemperature: "); Serial.println(sensor.readTemperature(), 2);
  
  delay(100000);

  DEBUG_PRINT("Function-End :: loop");
}

//
// Initialize Hardware Settings
//
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

  DEBUG_PRINT("Using ESP8266 - initializing EEPROM . . .");
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

void readConfiguration() {
  DEBUG_PRINT("Function-Begin :: readConfiguration");
  // Read the configuration from EEPROM

  String debug_msg;
  int memlocation = 0;
  
  // Wireless config
  DEBUG_PRINT( "Reading SSID . . ." );
  sysconfig.ssid = readEEPROM( memlocation, sysconfig.ssidSize );
  debug_msg = "SSD = [" + sysconfig.ssid + "]";
  DEBUG_PRINT( debug_msg );
  
  memlocation = memlocation + 1 + sysconfig.ssidSize;
  DEBUG_PRINT( "Reading PSK . . ." );
  sysconfig.pskey = readEEPROM( memlocation,sysconfig.pskeySize );
  debug_msg ="PSK = [" + sysconfig.pskey + "]";
  DEBUG_PRINT( debug_msg );
  
  // Device Info
  memlocation = memlocation + 1 + sysconfig.pskeySize;
  DEBUG_PRINT( "Reading sensor name . . ." );
  sysconfig.name = readEEPROM( memlocation, sysconfig.nameSize );
  debug_msg = "Sensor Name = [" + sysconfig.name + "]";
  DEBUG_PRINT( debug_msg );
  
  memlocation = memlocation + 1 + sysconfig.nameSize;
  DEBUG_PRINT( "Reading serial number . . ." );
  sysconfig.serialNo = readEEPROM( memlocation, sysconfig.serialNoSize );
  debug_msg = "Serial No. : [" + sysconfig.serialNo +"]";
  DEBUG_PRINT( debug_msg );
  memlocation = memlocation + 1 + sysconfig.serialNoSize;

  // Device Admin password hash
  DEBUG_PRINT( "Reading admin password hash . . ." );
  sysconfig.passwdHash = readEEPROM( memlocation, sysconfig.passwdHashSize );
  debug_msg = "Password Hash : [" + sysconfig.passwdHash + "]";
  DEBUG_PRINT( debug_msg );
  DEBUG_PRINT("Function-End :: readConfiguration");
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

  // Commit EEPROM
  DEBUG_PRINT("Commiting EEPROM changes . . ." );
  EEPROM.commit();
  
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

  DEBUG_PRINT( "Commiting EEPROM changes." );
  EEPROM.commit();


  DEBUG_PRINT( "Verifying EEPROM value saved . . ." );
  String verified = readEEPROM( start, size );
  DEBUG_PRINT( verified );
  
  DEBUG_PRINT("Function-End :: writeEEPROM");
}

void readSensors() {
  sensorTemp = sensor.readTemperature();
  sensorHumidity = sensor.readTemperature();
}


