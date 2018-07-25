/*
   Remote Temperature and Humidity Sendor

   Read the temperature and humidy and submit it to
   a web server.
*/

#include "debug.h"
#include "eprom.h"
#include "init.h"
#include "sensor.h"
#include "configuration.h"
#include <Adafruit_Si7021.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <TimedAction.h>
#include <Hash.h>
#include <FS.h>
#include "user_interface.h"

// System configuration
Configuration sysconfig;

// Sensor
bool SensorFound = false;
Adafruit_Si7021 sensor = Adafruit_Si7021();
int sensorTemp;
int sensorHumidity;

// Salt for password hash
#define SALT "akddjuemv"


#define WEBSERVER_PORT 80



// WiFi
#define APMODE_SSID "Sensor-Setup"
bool apMode = false;

// IP Address for SoftAP
IPAddress local_IP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);


// Set times action
TimedAction sensorThread = TimedAction(30000, readSensor);

// Set web server port number to 80
ESP8266WebServer server( WEBSERVER_PORT );

void handleRoot(){
  server.sendHeader("Location", "/index.html",true);   //Redirect to our html web page
  server.send(302, "text/plane","");
}

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
  readConfiguration( &sysconfig );

  // Setup Wireless Network Connection
  DEBUG_PRINT("Connecting to WiFi . . ." );
  connectWiFi();

  SPIFFS.begin();
  Serial.println("File System Initialized");

  // Set up Web Server
  DEBUG_PRINT( "Setting up weberser . . ." );
  server.onNotFound(handleWebRequests);
  server.on( "/", htmlRoot );
  server.on( "/login", htmlLogin );
  server.on( "/config", htmlConfig );
  server.on( "/sensor", htmlSensor );
  server.on( "/save", handleSave );

  // server.onNotFound( htmlError404 );

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




void handleWebRequests(){
  if(loadFromSpiffs(server.uri())) return;
  String message = "File Not Detected\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " NAME:"+server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.println(message);
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
  DEBUG_PRINT( message );
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

bool isAuthenticated() {
  DEBUG_PRINT("Function-Begin :: isAuthenticated");
  if (server.hasHeader("Cookie")) {
    Serial.print("Found cookie: ");
    String cookie = server.header("Cookie");
    Serial.println(cookie);
    if (cookie.indexOf("ESPSESSIONID=1") != -1) {
      Serial.println("Authentification Successful");
      return true;
    }
  }
  Serial.println("Authentification Failed");
  
  DEBUG_PRINT("Function-End :: isAuthenticated");
  return false;
  
}

void htmlLoginPage( String msg ) {
      // Displasy Login Page
      String html  = htmlPageOpen();
             html += htmlHEAD( "Sensor Setup" );
             html += "<div class=\"wrap\">";
             html += "<div class=\"avatar\">";
             html += "<img src=\"sensor.png\">";
             html += "</div>";
             html += "<form id=\"login\" action=\"login\" method=\"put\">";
             html += "<input type=\"text\" name=\"username\" placeholder=\"username\" required>";
             html += "<div class=\"bar\">";
             html += "<i></i>";
             html += "</div>";
             html += "<input type=\"password\" name=\"password\" placeholder=\"password\" required>";
             html += "<a href=\"\" class=\"forgot_link\">forgot ?</a>";
             // html += "<button onclick=\"document.getElementById('login').submit()\">Sign in</button>";
             html += "<input type=\"submit\" value=\"Sign in\">";
             html += "</form>";
             html += "</div>";
             html += htmlPageClose();

      DEBUG_PRINT( html );
      server.send ( 200, "text/html", html );
}


// Login Web Page
void htmlLogin() {
  DEBUG_PRINT("Function-Begin :: htmlLogin");

  String message;
  for (int i = 0; i < server.args(); i++) {
    message += "Arg no" + (String)i + " –> ";
    message += server.argName(i) + ": ";
    message += server.arg(i) + "\n";
  } 
  Serial.println("Server Arguments");
  Serial.println("================");
  Serial.println( message );
  
  if ( isAuthenticated() ) {
    server.sendHeader("Location", "/");
    server.sendHeader("Cache-Control", "no-cache");
    server.sendHeader("Set-Cookie", "ESPSESSIONID=1");
    server.send(301);
    Serial.println("Already logged in.");
  } else if (server.hasArg("username") && server.hasArg("password")) {
    Serial.println( server.arg("password"));
    String salted = SALT+server.arg("password");
    Serial.println( salted );
    String hashedPassword = sha1( salted );
    Serial.println( hashedPassword );
    if ((server.arg("username") == "Admin") && (sysconfig.passwdHash == hashedPassword)) {
      server.sendHeader("Location", "/");
      server.sendHeader("Cache-Control", "no-cache");
      server.sendHeader("Set-Cookie", "ESPSESSIONID=1");
      server.send(301);
      Serial.println("Log in Successful");
      return;    
    } else {
      Serial.println("Wrong password or username");
      htmlLoginPage("Authentication failed.");
    }
  } else {
    htmlLoginPage("");
  }

  
  DEBUG_PRINT("Function-End :: htmlLogin");

}

void handleSave() {
  String str= "Settings Saved ...\r\n";

  Serial.print("Number of args: ");
  Serial.println(server.args());
  Serial.print("Argument(0): ");
  Serial.println(server.arg(0));
  
  if (server.args() > 0 ) {
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      str += server.argName(i) + " = " + server.arg(i) + "\r\n";
    }
  }
  server.send(200, "text/plain", str.c_str());
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
  // retStr += "<meta http-equiv=\"refresh\" content=\"30\">";
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

// Redirect to specified page
void htmlRedirect( String page ) {
  server.sendHeader("Location", String( page ), true);
  server.send ( 302, "text/plain", "");
}

// Check if header is present and correct

