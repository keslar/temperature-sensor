/*
 * configuration.h
 * 
*/
#ifndef configurationH
#define configurationH

// 
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

void readConfiguration( Configuration* sysconfig );
void saveConfiguration( Configuration* sysconfig );
#endif
