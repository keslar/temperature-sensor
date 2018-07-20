/*
 * setup.h - setup and initialization rroutines
 */

#ifndef setupH
#define setupH

#include <Adafruit_Si7021.h>
#include "eprom.h"

#define BAUDRATE 115200
#define EEPROM_SIZE 1024

struct SensorObj
{
  String          sensorname;
  Adafruit_Si7021 Si7021;
  bool            present;
  float           temperature;
  float           humidity;
};

struct SensorObj initHardware( void );

#endif
