/*
 * MyWiFi.h
 * 
 */

#ifndef MyWiFiH
#define MyWiFiH
#include <ESP8266WiFi.h>

// WiFi
#define APMODE_SSID "Sensor-Setup"

// IP Address for SoftAP
IPAddress local_IP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

void initWiFi( String, String );

#endif
