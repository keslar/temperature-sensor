/*
 * EEPROM functions
 */

#ifndef epromH
#define epromH

#include <EEPROM.h>

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

void    initEEPROM( int );
String  readEEPROM (int, int );
void    writeEEPROM( int, int, String );

struct Configuration readConfiguration();
void writeConfiguration( struct Configuration );


#endif
