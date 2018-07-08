/*
 * Remote Temperature and Humidity Sendor
 * 
 * Read the temperature and humidy and submit it to 
 * a web server.
 */

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include "Adafruit_Si7021.h"

#define NULL 0

typedef struct WiFiConfig {
  String ssid;
  String pskey;
};

typedef struct SensorInfo {
  String sensorName;
  String SerialNo;
};


WiFiConfig WiFiNetwork = { "senorconfig", "none" };
SensorInfo Sensor = { "Sensor", "" };
bool APMode = false;

Adafruit_Si7021 sensor = Adafruit_Si7021();

//
// Setup Routine
//
void setup() {
  initHardware();
  
  
  //
  // WiFi Configuration
  //
  getWiFiConfig();

  //
  // Sensor Information
  //
  getSensorConfig();

  //
  // Setup Wireless Network Connection
  //
  connectWiFi();
  if ( WiFiNetwork.ssid == "sensorconfig" ) {
    apMode = true;
    // Start up in AP mode so sensor can be configured
    IPAddress local_IP(192,168,4,1);
    IPAddress gateway(192,168,4,1);
    IPAddress subnet(255,255,255,0);
    Serial.println("Set soft-AP configuration . . . ");
    Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
    Serial.print("Setting soft-AP . . . ");
    Serial.println(WiFi.softAP(WiFiNetwork.ssid) ? "Ready" : "Failed!");
    Serial.print("Soft-AP IP address = ");
    Serial.println(WiFi.softAPIP());
    if ( Sensor.serialno == "" ) {
      Sensor.serialno = WiFi.softAPmacAddress().c_str());
    }
  } else {
    // Connect to the WiFi Network
    Serial.println("Connecting to the WiFi network . . .");
    Serial.print("SSID :: ");
    Serial.println( WiFiNetwork.ssid );
    Serial.print("PSK  :: ");
    Serial.println( WiFiNetwork.pskey );
    WiFi.begin( WiFiNetwork.ssid, WiFiNetwork.pskey );

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.print("IP address = ");
    Serial.println(WiFi.localIP());
  }

}


void loop() {
  Serial.print("Humidity:    "); Serial.print(sensor.readHumidity(), 2);
  Serial.print("\tTemperature: "); Serial.println(sensor.readTemperature(), 2);
  delay(10000);
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
    while (true);
  }

  //
  // Setup EEPROM
  //
  EEPROM.begin(1024);
  EEPROM.commit();
}

//
// Read the WiFi configuraiton from EEPROM 
//
void getWiFiConfig() {
  String ssid;
  String pskey;
  char c;

  int memcount = 0;

  // Read the ssid from EEPROM
  do {
    c = EEPROM.read(memcount++);

    if ( c != NULL ) {
      ssid += (c);
    }
  } while ( c != NULL );

  // Read the pre-shared key
  do {
    c = EEPROM.read(memcount++);
    if ( c != NULL ) {
      pskey += (c);
    }
  } while ( c != NULL );


  if ( ssid.length() == 0 ) {
    Serial.println( "EEPROM was empty." );
    setWiFiConfig();
  } else {
    WiFiNetwork = { ssid, pskey };
  }

  setWiFiConfig();
}

//
// Write WiFi Configuration to EEPROM
//
void setWiFiConfig() {
  /*
   * Write the WiFi config to EEPROM
   */
  int h=0;
   
  // Write SSID to EEPROM
  for( int i=0; i <= WiFiNetwork.ssid.length(); i++ ) {
    EEPROM.write( i, WiFiNetwork.ssid.charAt(i) );
    EEPROM.write( i+1, NULL );
    h = i;
  }

  // NULL seperates values
  EEPROM.write( h++, NULL );

  //  Write PSK to EEPROM
  for( int j=0; j <= WiFiNetwork.pskey.length(); j++ ) {
      EEPROM.write( h+j, WiFiNetwork.pskey.charAt(j) );
      EEPROM.write( h+j+1, NULL );
  }
  // NULL seperates values
  EEPROM.write(  h+WiFiNetwork.pskey.length(), NULL );

  EEPROM.commit();
}

//
// Get Sensor Configuraiton
//
void getSensorInfo() {
  String sensorName;
  String serialno;
  String pskey;
  char c;

  int memcount = WiFiNetwork.ssid.length() + WiFiNetwork.pskey.length();

  // Read the ssid from EEPROM
  do {
    c = EEPROM.read(memcount++);

    if ( c != NULL ) {
      sensorName += (c);
    }
  } while ( c != NULL );

  // Read the pre-shared key
  do {
    c = EEPROM.read(memcount++);
    if ( c != NULL ) {
      serialno += (c);
    }
  } while ( c != NULL );


  if ( ssid.length() == 0 ) {
    Serial.println( "EEPROM was empty." );
    setSensorInfo();
  } else {
    Sensor = { sensorName, serialno };
  }

  setWiFiConfig();
  
}

//
// Set Sensor Configuration
//
void setSensorInfo() {
  
}

//
// Connect to WiFi network or run in AP mode
//
void connectWiFi() {
  if ( WiFiNetwork.ssid == "sensorconfig" ) {
    apMode = true;
    // Start up in AP mode so sensor can be configured
    IPAddress local_IP(192,168,4,1);
    IPAddress gateway(192,168,4,1);
    IPAddress subnet(255,255,255,0);
    Serial.println("Set soft-AP configuration . . . ");
    Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
    Serial.print("Setting soft-AP . . . ");
    Serial.println(WiFi.softAP(WiFiNetwork.ssid) ? "Ready" : "Failed!");
    Serial.print("Soft-AP IP address = ");
    Serial.println(WiFi.softAPIP());
    if ( Sensor.serialno == "" ) {
      Sensor.serialno = WiFi.softAPmacAddress().c_str());
    }
    setSensoInfo();
  } else {
    // Connect to the WiFi Network
    Serial.println("Connecting to the WiFi network . . .");
    Serial.print("SSID :: ");
    Serial.println( WiFiNetwork.ssid );
    Serial.print("PSK  :: ");
    Serial.println( WiFiNetwork.pskey );
    WiFi.begin( WiFiNetwork.ssid, WiFiNetwork.pskey );

    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.print("IP address = ");
    Serial.println(WiFi.localIP());
  }
}
