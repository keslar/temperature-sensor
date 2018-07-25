/*
 * MyWiFi.h
 * 
 */

#ifndef MyWiFiH
#define MyWiFiH
#include "debug.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

// WiFi
#define APMODE_SSID "Sensor-Setup"
#define NTP_LISTEN_PORT 2390
#define NTP_PACKET_SIZE 48
#define NTP_SERVERNAME  "time.nist.gov"

// IP Address for SoftAP
IPAddress local_IP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

void initWiFi( String, String );

void setTime( void );

#endif
