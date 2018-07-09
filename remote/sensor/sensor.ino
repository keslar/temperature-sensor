
/*
 * Remote Temperature and Humidity Sendor
 * 
 * Read the temperature and humidy and submit it to 
 * a web server.
 */

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <TimedAction.h>
#include <MD5.h>
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

char *Password;
unsigned char PasswordHash;
char *PasswordStr;

Adafruit_Si7021 sensor = Adafruit_Si7021();

int sensorTemp;
int sensorHumidity;

TimedAction = sensorThread( 30000, readSensors );

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

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
  // Read password hash;
  //
  getPassword();

  //
  // Setup Wireless Network Connection
  //
  connectWiFi();

}


void loop() {
  sensorThread.check();
 
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /5/on") >= 0) {
              Serial.println("GPIO 5 on");
              output5State = "on";
              digitalWrite(output5, HIGH);
            } else if (header.indexOf("GET /5/off") >= 0) {
              Serial.println("GPIO 5 off");
              output5State = "off";
              digitalWrite(output5, LOW);
            } else if (header.indexOf("GET /4/on") >= 0) {
              Serial.println("GPIO 4 on");
              output4State = "on";
              digitalWrite(output4, HIGH);
            } else if (header.indexOf("GET /4/off") >= 0) {
              Serial.println("GPIO 4 off");
              output4State = "off";
              digitalWrite(output4, LOW);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP8266 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 5  
            client.println("<p>GPIO 5 - State " + output5State + "</p>");
            // If the output5State is off, it displays the ON button       
            if (output5State=="off") {
              client.println("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/5/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 4  
            client.println("<p>GPIO 4 - State " + output4State + "</p>");
            // If the output4State is off, it displays the ON button       
            if (output4State=="off") {
              client.println("<p><a href=\"/4/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/4/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
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
    while (true);
  }

  //
  // Setup EEPROM
  //
  EEPROM.begin(1024);
  EEPROM.commit();
}

void readSensors() {
  sensorTemp = sensor.readTemperature();
  sensorHumidity = sensor.readTemperature();
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

  // Read the sensor name from EEPROM
  do {
    c = EEPROM.read(memcount++);

    if ( c != NULL ) {
      sensorName += (c);
    }
  } while ( c != NULL );

  // Read the serial number key
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
  
  int memcount = WiFiNetwork.ssid.length() + WiFiNetwork.pskey.length();

  // Write Sensor name
  Serial.print( "Setting sensor name to " );
  Serial.println( Sensor.sensorName );
  for( int i=0; i <= Sensor.sensorName.length(); i++ ) {
    EEPROM.write( memcount+i, Sensor.sensorName.charAt(i) );
    EEPROM.write( memcount+i+1, NULL );
  }
  memcount = memcount + Sensor.sensorName.length();

  // Write Sensor serial number
  Serial.print( "Setting sensor serial number to " );
  Serial.println( Sensor.SerialNo );

  for( int j=0; j <= Sensor.SerialNo.length(); j++ ) {
      EEPROM.write( memcount+j, Sensor.SerialNo.charAt(j) );
      EEPROM.write( memcount+j+1, NULL );
  }
  // NULL seperates values
  memcount = memcount + Sensor.SerialNo.length();
  EEPROM.write(  memcount++ , NULL );
}

//
// Get Pssword Hash from EEPROM
//
void getPassword() {
  int memcount = WiFiNetwork.ssid.length() + WiFiNetwork.pskey.length() + Sensor.sensorName.length() + Sensor.SerialNo.length();
  char c;

  // Read the sensor name from EEPROM
  do {
    c = EEPROM.read(memcount++);

    if ( c != NULL ) {
      PasswordStr += (c);
    }
  } while ( c != NULL );

}

//
// Save Password Hash to EEPROM
//
void setPassword() {
  
  int memcount = WiFiNetwork.ssid.length() + WiFiNetwork.pskey.length() + Sensor.sensorName.length() + Sensor.SerialNo.length();

  // Write Sensor name
  Serial.print( "Setting password hash to " );
  Serial.println( PasswordStr );
  for( int i=0; i <= PasswordStr.length(); i++ ) {
    EEPROM.write( memcount+i, PasswordStr.charAt(i) );
    EEPROM.write( memcount+i+1, NULL );
  }
  memcount = memcount + PasswordStr.length();

  EEPROM.commit();
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
    wifi_softap_set_dhcps_offer_option(OFFER_ROUTER, 0 ); 
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
