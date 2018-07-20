/*
 * MyWiFi.ino
 * 
 */

 #include "MyWiFi.h"

 
//
// Connect to WiFi network or run in AP mode
//
void initWiFi(String SSID, String PSKey ) {
    
  if ( SSID == APMODE_SSID ) {  
    // Start up in AP mode so sensor can be configured
    // wifi_softap_set_dhcps_offer_option(OFFER_ROUTER, 0 );
    Serial.println("Set soft-AP configuration . . . ");
    WiFi.mode(WIFI_AP_STA);
    Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

    Serial.print("Starting softAP . . . ");
    Serial.println( WiFi.softAP( SSID.c_str() ) ? "Ready" : "Failed!");
    Serial.print("Soft-AP IP address = ");
    Serial.println(WiFi.softAPIP());
  } else {
    // Connect to the WiFi Network
    Serial.println("Connecting to the WiFi network . . .");
    Serial.print("SSID :: ");
    Serial.println( SSID );
    Serial.print("PSK  :: ");
    Serial.println( PSKey );
    if ( PSKey == "" ) {
      WiFi.begin( SSID.c_str() );
    } else {
      WiFi.begin( SSID.c_str() , PSKey.c_str() );
    }


    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

    Serial.print("IP address = ");
    Serial.println(WiFi.localIP());
  }
}

