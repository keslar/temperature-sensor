/*
 * configuration.ino
 *
 */
#include "debug.h"
#include "configuration.h"

void readConfiguration( Configuration* sysconfig ) {
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

// Save the configuration to EEPROM
void saveConfiguration( Configuration* sysconfig ) {
  DEBUG_PRINT("Function-Begin :: saveConfiguration");

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

  DEBUG_PRINT("Function-End :: saveConfiguration");
}



