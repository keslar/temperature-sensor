/*
 * eeprom.ino
 *
 * EEPROM routines
 * 
*/
#include "debug.h"
#include "eprom.h"
#include <EEPROM.h>

// Initialize EEPROM
void initEEPROM() { 
  EEPROM.begin( EEPROM_SIZE );
}

// Read a string from the EEPROM
String readEEPROM (int start, int size ) {
  String  retString;
  int     memcount = start;
  char    c;

  for ( int i = memcount; i < (memcount + size); i++ ) {
    c = EEPROM.read( i );
    if ( c != 0 ) {
      retString += (c);
    } else {
      break;
    }
  }

  return retString;
}

// Write a value into the EEPROM
void writeEEPROM( int start, int size, String value ) {
  DEBUG_PRINT("Function-Begin :: writeEEPROM");
  DEBUG_PRINT( start );
  DEBUG_PRINT( size );
  DEBUG_PRINT( value );
  for (int i = 0; i < (value.length() < size ? value.length() : size); i++) {
    EEPROM.write(start + i, value.charAt(i));
  }

  EEPROM.write( start + (value.length() < size ? value.length() : size), 0 );

  DEBUG_PRINT( "Commiting EEPROM changes." );
  EEPROM.commit();


  DEBUG_PRINT( "Verifying EEPROM value saved . . ." );
  String verified = readEEPROM( start, size );
  DEBUG_PRINT( verified );

  DEBUG_PRINT("Function-End :: writeEEPROM");
}

