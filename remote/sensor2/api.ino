/*
 * api.ino
 * 
 * API intiailization and routines
 * 
 */
#include "api.h"
#include <ESP8266WebServer.h>

String dataType = "text/json";

//
// Initialize the API
void initAPI( ESP8266WebServer *server ) {
  webserver = server;
  
  server->on("/sensor", getSensors );
  server->on("/sensor/1", getSensor1 );
  server->on("/sensor/2", getSensor2 );

  server->on("/authentication", isAuthenticated );
  server->on("/authenticatation/login", Authenticate );

  server->on("/configuration", getConfiguration );
  server->on("/configuration/ssid", getSSID );
  server->on("/configuration/ssid/set", setSSID );
  server->on("/configuration/pskey", getPSKEY );
  server->on("/configuration/pskey/set", setPSKEY );
  server->on("/configuration/name", getName );
  server->on("/configuration/name/set", setName );
  server->on("/configuration/serialNo", getSerialNo );
  server->on("/configuration/serialNo/set", setSerial );
  server->on("/configuration/passwdHash", getPasswdHash );
  server->on("/configuration/name/set", setPasswdHash );
  
  
  String  passwdHash;
  int     passwdHashSize  = 40;
}

//
// Get sensor reading, send json document in response
//     
void getSensors() {

}

void getSensor(int i) {
  
}

void getSensor1() {
  getSensor(1);
}

void getSensor2() {
  getSensor(2);
}


void isAuthenticated() {
  
}

void Authenticate() {
  
}

void getConfiguration() {
  
}

void getSSID() {
  
}

void setSSID() {
  
}

void getPSKEY() {
  
}

void setPSKEY() {
  
}

void getName() {
  
}

void setName() {
  
}

void getSerialNo() {
  
}

void setSerial() {
  
}

void getPasswdHash() {
  
}

void setPasswdHash() {
  
}

