/*
 * 
 * Skeleton Sketch
 * 
 */

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <TimedAction.h>
#include <Adafruit_Si7021.h>

#define DEBUG

#ifdef DEBUG
 #define DEBUG_PRINT(x)  Serial.println (x)
#else
 #define DEBUG_PRINT(x)
#endif

/* 
 *  Configuration and EEPROM settings
 */
#define BAUDRATE 115200 
#define EEPROM_SIZE 1024
#define WEBSERVER_PORT 80

struct Configuration {
  // WiFi Settings
  String  ssid            = "None";
  int     ssidSize        = 32;    // max: 32
  String  psk             = "wifi password";
  int     pskSize         = 64;     // max: 64
  // Device Info
  String  name            = "";
  int     nameSize        = 64;
  String  serialNo        = "";
  int     serialNoSize    = 17;
  // Admin password
  String  passwdHash      = "";
  int     passwdHashSize  = 12;
};
Configuration sysconfig;

// Are we in Soft AP mode
bool APMode = false;

// Sensor
Adafruit_Si7021 sensor = Adafruit_Si7021();
#define SENSORPOLLINTERVAL 300000
int sensorTemp;
int sensorHumidity;

// Set web server port number to 80
DEBUG_PRINT("Starting webserver on port" + WEBSERVER_PORT + " . . .");
WiFiServer server(WEBSERVER_PORT);
// Variable to store the HTTP request
String header;

// Setup Timed Action
DEBUG_PRINT( "Registering sensor read thread . . ." );
TimedAction = sensorThread( SENSORPOLLINTERVAL, readSensors );

void setup() {
  DEBUG_PRINT("Function-Begin :: setup");
  
  // Load the configuration from EEPROM
  DEBUG_PRINT("Loading configuration . . . ");
  loadConfiguration();
  
  // Initialize any hardware if necessary
  DEBUG_PRINT("Initializing hardware . . .");
  initHardware();

  // Connect to WiFi network or start in SoftAP mode
  DEBUG_PRINT( "Connecting to WiFi network . . ." );

  // Register timed actions
  
  // Start Webserver

  DEBUG_PRINT("Function-End :: setup");
}

void loop() {
  // put your main code here, to run repeatedly:
  DEBUG_PRINT("Function-Begin :: loop");

  DEBUG_PRINT("Function-End :: loop");
}

void initHardware() {
  DEBUG_PRINT("Function-Begin :: initHardware");
  // put any hardware initialization code here:
  
  //
  // Initialize serial output
  //
  DEBUG_PRINT("Setting baudrate to " + BAUDRATE + " . . ." );
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
  #ifdef ESP8266_h
    DEBUG_PRINT("Using ESP8266 - initializing EEPROM to " + EEPROMSIZE + " bytes . . .");
    EEPROM.begin( EEPROM_SIZE );
  #endif

  //
  // Check for sensor
  //
  DEBUG_PRINT( "Checking for Si7021 sensor . . ." ); 
  if (!sensor.begin()) {
    Serial.println("Did not find Si7021 sensor!");
    // Hang if not found
    while (true);
  }
  DEBUG_PRINT("Sensor found.");

  DEBUG_PRINT("Function-End :: initHardware");
}

void clearConfiguration() {
  // Clear the configuraiton from the EEPROM

  // Write NULLs into the EEPROM
  for( int i = 0; i < EEPROM_SIZE; i++ ) {
    EEPROM.write( i, 0x0 );
  }

  #ifdef ESP8266_h
    EEPROM.commit();
  #endif

}

void loadConfiguration() {
  // Load Configuraiton from EEPROM

  int memlocation = 0;
  
  // Wireless SSID
  sysconfig.ssid        = readEEPROM( memlocation, sysconfig.ssidSize );
  memlocation = memlocation + 1 + sysconfig.ssidSize;
  sysconfig.psk         = readEEPROM( memlocation, sysconfig.pskSize );
  memlocation = memlocation + 1 + sysconfig.pskSize;
  
  // Device Info
  sysconfig.name        = readEEPROM( memlocation, sysconfig.nameSize );
  memlocation = memlocation + 1 + sysconfig.nameSize;
  sysconfig.serialNo    = readEEPROM( memlocation,sysconfig.serialNoSize );
  memlocation = memlocation + 1 + sysconfig.serialNoSize;
  
  // Password hash for device admin account
  sysconfig.passwdHash  = readEEPROM( memlocation, sysconfig.passwdHashSize );
}

void saveConfiguration() {
  // Save the configuration to EEPROM

  int memlocation = 0;
  
  // Wireless config
  writeEEPROM( memlocation, sysconfig.ssidSize,sysconfig.ssid );
  memlocation = memlocation + 1 + sysconfig.ssidSize;
  writeEEPROM( memlocation,sysconfig.pskSize,sysconfig.psk );
  memlocation = memlocation + 1 + sysconfig.pskSize;

  // Device Info
  writeEEPROM( memlocation, sysconfig.nameSize, sysconfig.name );
  memlocation = memlocation + 1 + sysconfig.nameSize;
  writeEEPROM( memlocation, sysconfig.serialNoSize, sysconfig.serialNo );
  memlocation = memlocation + 1 + sysconfig.serialNoSize;

  // Device Admin password hash
  writeEEPROM( memlocation, sysconfig.passwdHashSize, sysconfig.passwdHash );
  
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

  #ifdef ESP8266_h
    EEPROM.commit();
  #endif

  return retString;
}

void writeEEPROM( int start, int size, String value ) {
  // Write a value into the EEPROM
  
  for (int i=0; i<(value.length()<size?value.length():size); i++) {
    EEPROM.write(start+i,value.charAt(i));
  }
  EEPROM.write( (value.length()<size?value.length():size), 0 );

  #ifdef ESP8266_h
    EEPROM.commit();
  #endif
}

void readSensors() {
  // Read the values from the sensors
  sensorTemp = sensor.readTemperature();
  sensorHumidity = sensor.readTemperature();
}
