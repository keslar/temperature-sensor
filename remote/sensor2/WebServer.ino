/*
 * WebServer.ino
 * 
 * Webserver functions
 */
#include <ESP8266WebServer.h>
#include "WebServer.h"

ESP8266WebServer *webserver;

//
// Initialize the Web Server
//
void initWebServer(ESP8266WebServer *server ) {
  // Initialize the filesystem so Web Server can server images and html files
  SPIFFS.begin();
  Serial.println("File System Initialized");

  // Start a webserver
  Serial.print("Start webserver on port " );
  Serial.print( WEBSERVER_PORT );
  Serial.println( " . . ." );
  server = new ESP8266WebServer( WEBSERVER_PORT );
  webserver = server;
  
  // Setup page parsing
  // if URI not defined not found, it might be a file
  server->onNotFound( handleWebRequests );
}

// 
// Parse the URI and send file through web server
//

bool loadFromSpiffs( String path){
  String dataType = "text/plain";
  if(path.endsWith("/")){
    path += "index.htm";
  }
 
  if(path.endsWith(".src")) {
    path = path.substring(0, path.lastIndexOf("."));
  } else if(path.endsWith(".html")) {
    dataType = "text/html";
  } else if(path.endsWith(".htm")) {
    dataType = "text/html";
  } else if(path.endsWith(".css")) {
    dataType = "text/css";
  } else if(path.endsWith(".js")) {
    dataType = "application/javascript";
  } else if(path.endsWith(".png")) {
    dataType = "image/png";
  } else if(path.endsWith(".gif")) {
    dataType = "image/gif";
  } else if(path.endsWith(".jpg")) {
    dataType = "image/jpeg";
  } else if(path.endsWith(".ico")) {
    dataType = "image/x-icon";
  } else if(path.endsWith(".xml")) {
    dataType = "text/xml";
  } else if(path.endsWith(".pdf")) {
    dataType = "application/pdf";
  } else if(path.endsWith(".zip")) {
    dataType = "application/zip";
  }
  
  File dataFile = SPIFFS.open(path.c_str(), "r");
/*
  // If URI is a directory, append index.html to path and try to open it
  if (dataFile.isDirectory()) {
    path += "/index.htm";
    dataType = "text/html";
    dataFile = SPIFFS.open(path.c_str());
  }
*/

  if ( !dataFile ) {
    return false;
  } 
  
  if (webserver->hasArg("download")) {
    dataType = "application/octet-stream";
  }
  
  if (webserver->streamFile(dataFile, dataType) != dataFile.size()) {
    Serial.println("Sent less data than expected.");
  }
 
  dataFile.close();
  return true;
}

//
// Try to server the URI from the file system
//
void handleWebRequests(){
  if(!(loadFromSpiffs( webserver->uri()))) {
    Serial.print("URI: [");
    Serial.print(webserver->uri()); 
    Serial.println("] - not found.");
    loadFromSpiffs("/error-404.html");
  }
}

//
// API functions from here on down
//

