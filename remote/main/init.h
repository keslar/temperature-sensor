/*
 * init.h
 *
 * Initialization routine header file
 * 
*/
#ifndef initH
#define initH

#define BAUDRATE 115200
#define SENSORPOLLINTERVAL 300000

bool SensorFound = false;
Adafruit_Si7021 sensor;


bool initHardware( void );
#endif
