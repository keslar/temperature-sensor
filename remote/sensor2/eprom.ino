/*
 * EEPROM Functions
 */

#include "eprom.h"

// Set the size of the EEPROM to be used
void initEEPROM( int size ) {
  DEBUG_PRINT( "Function-Begin :: initEEPROM()" );
  Serial.println( "Caching " + String(size) + " bytes from EEPROM . . ." );
  EEPROM.begin( size );  
  DEBUG_PRINT( "Function-End :: initEEPROM()" );
}

// Read from EEPROM starting from start to the length
//   or a 0x0 is encountered.
String  readEEPROM (int start, int length ) {
  DEBUG_PRINT("Function-Begin :: readEEPROM");
  String  retString;
  int     memcount = start;
  char    c;

  Serial.print( "Retirving value from EEPROM, starting at ");
  Serial.print( start );
  Serial.print(" with a length of ");
  Serial.print(String(length));
  Serial.println( " . . ." );
  
  for ( int i = memcount; i < (memcount + length); i++ ) {
    c = EEPROM.read( i );
    if ( c != 0 ) {
      retString += (c);
    } else {
      break;
    }
  }
  Serial.print("Value = [");
  Serial.print(retString);
  Serial.println("]");

  DEBUG_PRINT("Function-End :: readEEPROM");
  return retString;  
}

// Write a value to EEPROM
void writeEEPROM( int start, int length, String value ) {
  DEBUG_PRINT("Function-Begin :: writeEEPROM");
  DEBUG_PRINT( start );
  DEBUG_PRINT( length );
  DEBUG_PRINT( value );
  for (int i = 0; i < (value.length() < length ? value.length() : length); i++) {
    EEPROM.write(start + i, value.charAt(i));
  }

  EEPROM.write( start + (value.length() < length ? value.length() : length), 0 );

  DEBUG_PRINT( "Commiting EEPROM changes." );
  EEPROM.commit();


  DEBUG_PRINT( "Verifying EEPROM value saved . . ." );
  String verified = readEEPROM( start, length );
  DEBUG_PRINT( verified );

  DEBUG_PRINT("Function-End :: writeEEPROM");
}

// Read the device configuration
struct Configuration readConfiguration() {
  DEBUG_PRINT("Function-Begin :: readConfiguration");
  
  String debug_msg;
  int memlocation = 0;

  Configuration sysconfig;

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
  return sysconfig;
}

// Save the device configuration
void writeConfiguration( Configuration sysconfig ) {
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

