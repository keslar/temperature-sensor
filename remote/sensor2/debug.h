/*
 * Debugging routines
 */

#ifndef debugH
#define debugH

#ifdef DEBUG
#define DEBUG_PRINT(x)  Serial.println(x)
#else
#define DEBUG_PRINT(x)
#endif  

#endif
