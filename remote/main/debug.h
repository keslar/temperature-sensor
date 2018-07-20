/*
 * debug.h
 *
 * Setting up debuging
 * 
*/
#ifndef debugH
#define debugH

#define DEBUG

#ifdef DEBUG
#define DEBUG_PRINT(x)  Serial.println(x)
#else
#define DEBUG_PRINT(x)
#endif

#endif
