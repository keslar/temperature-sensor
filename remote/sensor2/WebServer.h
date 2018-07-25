/*
 * WebServer.h
 * 
 * Functions and parameters for the web server
 * 
 */

#ifndef WebServerH
#define WebServerH

#include "debug.h"
#include <ESP8266WebServer.h>
#include <FS.h>

// Web Server port
#define WEBSERVER_PORT 80

void initWebServer( ESP8266WebServer * );

#endif
