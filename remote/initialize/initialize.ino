/*
 * 
 * Remote Temperature and Humidity Sendor
 * 
 * Initialize EEPROM and test sensor
 * 
 */

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include "Adafruit_Si7021.h"

#define EPROM_SIZE 1024

typedef struct WiFiConfig {
  String ssid;
  String pskey;
};

typedef struct SensorInfo {
  String sensorName;
  String SerialNo;
};

WiFiConfig WiFiNetwork = { "senorconfig", "" };
SensorInfo Sensor = { "Sensor", WiFi.macAddress() };
bool APMode = false;
bool SensorFound = true;

Adafruit_Si7021 sensor = Adafruit_Si7021();

void setup() {
  initHardware();
  setConfig();
}

void loop() {
  // put your main code here, to run repeatedly:
  if ( SensorFound ) {
    Serial.print("Humidity:    "); Serial.print(sensor.readHumidity(), 2);
    Serial.print("\tTemperature: "); Serial.println(sensor.readTemperature(), 2);
    delay(10000);
  } else {
    while (true);
  }
}

//
// Initialize Hardware Settings
//
void initHardware() {
  //
  // Initialize serial output
  //
  Serial.begin(115200);
  
  // wait for serial port to open
  while (!Serial) {
    delay(10);
  }

  //
  // Check for sensor
  // 
  if (!sensor.begin()) {
    Serial.println("Did not find Si7021 sensor!");
    SensorFound = false;
  }

  //
  // Initialize EEPROM
  //
  Serial.println( "Initializing the EEPROM . . ." );
  EEPROM.begin(EPROM_SIZE);
  for( int i = 0; i < EPROM_SIZE; i++ ) {
    EEPROM.write( i, 0 );
  }
  
  EEPROM.commit();
}

//
// Write Configuraiton to EEPROM
//
void setConfig() {
  int h=0;
  Serial.println( "Writing default configuration to EEPROM . . ." );
   
  // Write SSID to EEPROM
  Serial.print( "Setting SSID to " );
  Serial.println( WiFiNetwork.ssid );
  for( int i=0; i <= WiFiNetwork.ssid.length(); i++ ) {
    EEPROM.write( i, WiFiNetwork.ssid.charAt(i) );
    EEPROM.write( i+1, NULL );
    h = i;
  }
  

  // NULL seperates values
  EEPROM.write( h++, NULL );

  //  Write PSK to EEPROM
  Serial.print( "Setting PSK to " );
  Serial.println( WiFiNetwork.pskey );
  for( int j=0; j <= WiFiNetwork.pskey.length(); j++ ) {
      EEPROM.write( h+j, WiFiNetwork.pskey.charAt(j) );
      EEPROM.write( h+j+1, NULL );
  }
  // NULL seperates values
  h = h+WiFiNetwork.pskey.length();
  EEPROM.write(  h++ , NULL );

  // Write Sensor name
  Serial.print( "Setting sensor name to " );
  Serial.println( Sensor.sensorName );
  for( int i=0; i <= Sensor.sensorName.length(); i++ ) {
    EEPROM.write( h+i, Sensor.sensorName.charAt(i) );
    EEPROM.write( h+i+1, NULL );
  }
  h = h + Sensor.sensorName.length();

  // Write Sensor serial number
  Serial.print( "Setting sensor serial number to " );
  Serial.println( Sensor.SerialNo );

  for( int j=0; j <= Sensor.SerialNo.length(); j++ ) {
      EEPROM.write( h+j, Sensor.SerialNo.charAt(j) );
      EEPROM.write( h+j+1, NULL );
  }
  // NULL seperates values
  h = h + Sensor.SerialNo.length();
  EEPROM.write(  h++ , NULL );
  EEPROM.commit();
}

