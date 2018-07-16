/*
   Remote Temperature and Humidity Sendor

   Read the temperature and humidy and submit it to
   a web server.
*/

#include <EEPROM.h>

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
// #include <WiFiClient.h>
#include <TimedAction.h>
#include <Hash.h>
#include "Adafruit_Si7021.h"
#include "user_interface.h"

#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(x)  Serial.println(x)
#else
#define DEBUG_PRINT(x)
#endif


/*
    Configuration and EEPROM settings
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

// Set default configuration values
/*
  sysconfig.ssid = "Sensor-Setup";
  sysconfig.pskey = "";
  sysconfig.name = "undefined";
  sysconfig.serialNo = WiFi.macAddress();

  String salted = strcat(SALT,"admin");
  sysconfig.passwdHash = sha1( salted );
*/

// WiFi
#define APMODE_SSID "Sensor-Setup"
bool apMode = false;

// IP Address for SoftAP
IPAddress local_IP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

// Sensor
bool SensorFound = false;
Adafruit_Si7021 sensor = Adafruit_Si7021();
#define SENSORPOLLINTERVAL 300000
int sensorTemp;
int sensorHumidity;

// Set times action
void readSensor(void);
TimedAction sensorThread = TimedAction(30000, readSensor);

// Set web server port number to 80
ESP8266WebServer server( WEBSERVER_PORT );

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
  DEBUG_PRINT("Connecting to WiFi . . ." );
  connectWiFi();

  // Set up Web Server
  DEBUG_PRINT( "Setting up weberser . . ." );
  server.on( "/", htmlRoot );
  server.on( "/login", htmlLogin );
  server.on( "/config", htmlConfig );
  server.on( "/sensor", htmlSensor );

  server.onNotFound( htmlError404 );

  DEBUG_PRINT( "Staring webserver . . ." );
  server.begin();


  Serial.print("IP Address = " );
  Serial.println( WiFi.softAPIP() );
  
  DEBUG_PRINT("Function-End :: setup");
}


void loop() {
  // DEBUG_PRINT("Function-Begin :: loop");

  // DEBUG_PRINT("Is it time to check the sensor?");
  sensorThread.check();
  
  // DEBUG_PRINT("Check for web client request . . .");
  server.handleClient();
  
  // DEBUG_PRINT("Function-End :: loop");
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
  sysconfig.pskey = readEEPROM( memlocation, sysconfig.pskeySize );
  debug_msg = "PSK = [" + sysconfig.pskey + "]";
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
  debug_msg = "Serial No. : [" + sysconfig.serialNo + "]";
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
  writeEEPROM( memlocation, sysconfig.ssidSize, sysconfig.ssid );
  memlocation = memlocation + 1 + sysconfig.ssidSize;
  DEBUG_PRINT( "Writing PSK . . ." );
  writeEEPROM( memlocation, sysconfig.pskeySize, sysconfig.pskey );
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

  for ( int i = memcount; i < (memcount + size); i++ ) {
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
  for (int i = 0; i < (value.length() < size ? value.length() : size); i++) {
    EEPROM.write(start + i, value.charAt(i));
  }

  EEPROM.write( start + (value.length() < size ? value.length() : size), 0 );

  DEBUG_PRINT( "Commiting EEPROM changes." );
  EEPROM.commit();


  DEBUG_PRINT( "Verifying EEPROM value saved . . ." );
  String verified = readEEPROM( start, size );
  DEBUG_PRINT( verified );

  DEBUG_PRINT("Function-End :: writeEEPROM");
}

//
// Connect to WiFi network or run in AP mode
//
void connectWiFi() {
  if ( sysconfig.ssid == APMODE_SSID ) {
    apMode = true;
    const char *ssid;
    ssid = sysconfig.ssid.c_str();
    // Start up in AP mode so sensor can be configured
    // wifi_softap_set_dhcps_offer_option(OFFER_ROUTER, 0 );
    Serial.println("Set soft-AP configuration . . . ");
    WiFi.mode(WIFI_AP_STA);
    Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

    Serial.print("Starting softAP . . . ");
    Serial.println( WiFi.softAP( ssid ) ? "Ready" : "Failed!");
    Serial.print("Soft-AP IP address = ");
    Serial.println(WiFi.softAPIP());
  } else {
    // Connect to the WiFi Network
    Serial.println("Connecting to the WiFi network . . .");
    Serial.print("SSID :: ");
    Serial.println( sysconfig.ssid );
    Serial.print("PSK  :: ");
    Serial.println( sysconfig.pskey );
    if ( sysconfig.pskey = "" ) {
      WiFi.begin( sysconfig.ssid.c_str() );
    } else {
      WiFi.begin( sysconfig.ssid.c_str() , sysconfig.pskey.c_str() );
    }


    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.print("IP address = ");
    Serial.println(WiFi.localIP());
  }
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

// Display an HTML error 404 - File not Found
void htmlError404() {
  DEBUG_PRINT("Function-Begin :: htmlError404");
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for ( uint8_t i = 0; i < server.args(); i++ ) {
    message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
  }

  server.send ( 404, "text/plain", message );

  DEBUG_PRINT("Function-End :: htmlError404");

}

// Root web page
void htmlRoot() {
  DEBUG_PRINT("Function-Begin :: htmlRoot");

  String html  = htmlPageOpen();
  html += htmlHEAD( "Sensor Setup" );
  html += "Hello world!!";
  html += htmlPageClose();

  DEBUG_PRINT( html );

  server.send ( 200, "text/html", html );
  DEBUG_PRINT("Function-End :: htmlRoot");

}

// Login Web Page
void htmlLogin() {
  DEBUG_PRINT("Function-Begin :: htmlLogin");
  String html  = htmlPageOpen();
         html += htmlHEAD( "Sensor Setup" );
         html += "<div class=\"wrap\">";
         html += "<div class=\"avatar\">";
         // html += "<img src=\"http://cdn.ialireza.me/avatar.png\">";
         html += "</div>";
         html += "<input type=\"text\" placeholder=\"username\" required>";
         html += "<div class=\"bar\">";
         html += "<i></i>";
         html += "</div>";
         html += "<input type=\"password\" placeholder=\"password\" required>";
         html += "<a href=\"\" class=\"forgot_link\">forgot ?</a>";
         html += "<button>Sign in</button>";
         html += "</div>";
         html += htmlPageClose();

  DEBUG_PRINT( html );

  server.send ( 200, "text/html", html );

  
  DEBUG_PRINT("Function-End :: htmlLogin");

}

// Configuraiton Webpage
void htmlConfig() {
  DEBUG_PRINT("Function-Begin :: htmlConfig");
  DEBUG_PRINT("Function-End :: htmlConfig");

}

// Sensor Webpage - display the current sensor readings
void htmlSensor() {
  DEBUG_PRINT("Function-Begin :: htmlSensor");

  // Construct WebPage
  String message;
  
  DEBUG_PRINT("Function-End :: htmlSensor");
}

String htmlPageOpen() {
  DEBUG_PRINT("Function-Begin :: htmlPageOpen");
  String retStr  = "<!DOCTYPE html>";
  retStr += "<HTML>";

  return retStr;
  DEBUG_PRINT("Function-End :: htmlPageopen");
}

String htmlPageClose() {
  DEBUG_PRINT("Function-Begin :: htmlPageClose");
  return "</BODY></HTML>";
  DEBUG_PRINT("Function-End :: htmlPageClose");
}

String htmlHEAD( String pageTitle ) {
  DEBUG_PRINT("Function-Begin :: htmlHEAD");
  String  retStr  = "<HEAD>";
  retStr += "<TITLE>" + pageTitle + "</TITLE>";
  retStr += "<meta charset=\"UTF-8\">";
  retStr += "<meta name=\"description\" content=\"SensorSetup\">";
  retStr += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
  retStr += "<meta http-equiv=\"refresh\" content=\"30\">";
  // Insert style here
  retStr += "<STYLE>";
  retStr += "@font-face {";
  retStr += "font-family: 'Lato';";
  retStr += "font-style: normal;";
  retStr += "font-weight: 400;";
  retStr += "src: local('Lato Regular'), local('Lato-Regular'), url(http://themes.googleusercontent.com/static/fonts/lato/v7/qIIYRU-oROkIk8vfvxw6QvesZW2xOQ-xsNqO47m55DA.woff) format('woff');";
  retStr += "}";
  retStr += "body {";
  retStr += "background: #448ed3 ;";
  retStr += "font-family: 'Lato' ;";
  retStr += "}";
  retStr += ".wrap {";
  retStr += "width:250px;";
  retStr += "height: auto;";
  retStr += "margin: auto;";
  retStr += "margin-top: 10%;";
retStr += "}";
retStr += ".avatar {";
  retStr += "width: 100%;";
  retStr += "margin: auto;";
  retStr += "width: 65px;";
  retStr += "border-radius: 100px;";
  retStr += "height: 65px;";
  retStr += "background: #448ed3 ;";
  retStr += "position: relative;";
  retStr += "bottom: -15px;";
  retStr += "}";
  retStr += ".avatar img {";
  retStr += "width: 55px;";
  retStr += "height: 55px;";
  retStr += "border-radius: 100px;";
  retStr += "margin: auto;";
  retStr += "border:3px solid #fff;";
  retStr += "display: block;";
  retStr += "}";
  retStr += ".wrap input {";
  retStr += "border: none;";
  retStr += "background: #fff;";
  retStr += "font-family:Lato ;";
  retStr += "font-weight:700 ;";
  retStr += "display: block;";
  retStr += "height: 40px;";
  retStr += "outline: none;";
  retStr += "width: calc(100% - 24px) ;";
  retStr += "margin: auto;";
  retStr += "padding: 6px 12px 6px 12px;";
  retStr += "}";
  retStr += ".bar {";
  retStr += "width: 100%;";
  retStr += "height: 1px;";
  retStr += "background: #fff ;";
  retStr += "}";
  retStr += ".bar i {";
  retStr += "width: 95%;";
  retStr += "margin: auto;";
  retStr += "height: 1px ;";
  retStr += "display: block;";
  retStr += "background: #d1d1d1;";
  retStr += "}";
  retStr += ".wrap input[type='text'] {";
  retStr += "border-radius: 7px 7px 0px 0px ;";
  retStr += "}";
  retStr += ".wrap input[type='password'] {";
  retStr += "border-radius: 0px 0px 7px 7px ;";
  retStr += "}";
  retStr += ".forgot_link {";
  retStr += "color: #83afdf ;";
  retStr += "color: #83afdf;";
  retStr += "text-decoration: none;";
  retStr += "font-size: 11px;";
  retStr += "position: relative;";
  retStr += "left: 193px;";
  retStr += "top: -36px;";
  retStr += "}";
  retStr += ".wrap button {";
  retStr += "width: 100%;";
  retStr += "border-radius: 7px;";
  retStr += "background: #b6ee65;";
  retStr += "text-decoration: center;";
  retStr += "border: none;";
  retStr += "color: #51771a;";
  retStr += "margin-top:-5px;";
  retStr += "padding-top: 14px;";
  retStr += "padding-bottom: 14px;";
  retStr += "outline: none;";
  retStr += "font-size: 13px;";
  retStr += "border-bottom: 3px solid #307d63;";
  retStr += "cursor: pointer;";
  retStr += "}";
  retStr += "</STYLE>";
  retStr += "</HEAD>";
  retStr += "<BODY>";

  return retStr;

  DEBUG_PRINT("Function-End :: htmlHEAD");
}

String htmlPageHeader() {
  DEBUG_PRINT("Function-Begin :: htmlPageHeader");
  String retStr  = "";

  return retStr;
  DEBUG_PRINT("Function-End :: htmlPageHeader");
}

String htmlPageFooter() {
  DEBUG_PRINT("Function-Begin :: htmlPageFooter");
  String retStr  = "";

  return retStr;
  DEBUG_PRINT("Function-End :: htmlFooter");
}

