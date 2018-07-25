/*
 * api.h
 * 
 * API URI's and functions
 * 
 */

#ifndef apiH
#define apiH

#include "debug.h"
#include <ESP8266WebServer.h>
 
void initAPI( ESP8266WebServer *);
void getSensor( int );
void getSensor1( void );
void getSensor2( void );
void isAuthenticated( void );
void Authenticate( void );
void getConfiguration( void );
void getSSID( void );
void setSSID( void );
void getPSKEY( void );
void setPSKEY( void );
void getName( void );
void setName( void );
void getSerialNo( void );
void setSerial( void );
void getPasswdHash( void );
void setPasswdHash( void );
#endif
