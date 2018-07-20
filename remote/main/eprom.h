/*
 * eprom.h
 *
 * EEPROM routine header file
 * 
*/
#ifndef epromH
#define epromH

#define EEPROM_SIZE 1024

void    initEEPROM( void );
String  readEEPROM (int, int );
void    writeEEPROM( int, int, String );
#endif
