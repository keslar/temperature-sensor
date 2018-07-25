/*
 * sensor2.ino
 * 
 * First rewrite of the remote sensor code.
 */

#define DEBUG
#include "debug.h"
#include "setup.h"
#include "MyWiFi.h"
#include "WebServer.h"
#include "api.h"

struct SensorObj Sensor;
struct Configuration Device;

ESP8266WebServer *server;

void setup() {
  DEBUG_PRINT("Function-Begin :: setup()");
  
  // Initialize the hardware
  Serial.println("Initializing the hardware . . ." );
  Sensor = initHardware();

  // Read the device configuration from EEPROM
  Device = readConfiguration();

  // Connect to WiFi Network or Start up in AP mode
  initWiFi( Device.ssid, Device.pskey );

  // Set Time from NTP
  setTime();
  
  
  // Setup Web Server
  initWebServer( server );

  // Setup web API URI's
 // initAPI( webserver );
    
  DEBUG_PRINT("Function-End :: setup()");
}

void loop() {
  // put your main code here, to run repeatedly:

}
