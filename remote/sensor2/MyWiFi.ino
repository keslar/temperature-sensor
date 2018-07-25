/*
 * MyWiFi.ino
 * 
 */

#include "MyWiFi.h"

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;

//buffer to hold incoming and outgoing packets
byte packetBuffer[ NTP_PACKET_SIZE];

// IP Address of the time server
IPAddress timeServerIP;
 
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

//
// send an NTP request to the time server at the given address
//
unsigned long sendNTPpacket(IPAddress& address) {
  Serial.println("sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}

//
// Set the date and time using NTP
//
void setTime() {
  Serial.println("Setting time through NTP . . .");
  if (Device.ssid != APMODE_SSID ) {
    Serial.println("Starting UDP");
    udp.begin( NTP_LISTEN_PORT );
    Serial.print("Local port: ");
    Serial.println(udp.localPort());
    
    // Get IP address of timeserver
    WiFi.hostByName(NTP_SERVERNAME, timeServerIP); 
    
    // send an NTP packet to a time server
    sendNTPpacket(timeServerIP);
    
    // wait to see if a reply is available
    delay(1000);
  
    int cb = udp.parsePacket();
    if (!cb) {
      Serial.println("no packet yet");
    } else {
      Serial.print("packet received, length=");
      Serial.println(cb);
      // We've received a packet, read the data from it
      udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

      //the timestamp starts at byte 40 of the received packet and is four bytes,
      // or two words, long. First, esxtract the two words:
      unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
      unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
      // combine the four bytes (two words) into a long integer
      // this is NTP time (seconds since Jan 1 1900):
      unsigned long secsSince1900 = highWord << 16 | lowWord;
      Serial.print("Seconds since Jan 1 1900 = " );
      Serial.println(secsSince1900);

      // now convert NTP time into everyday time:
      Serial.print("Unix time = ");
      // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
      const unsigned long seventyYears = 2208988800UL;
      // subtract seventy years:
      unsigned long epoch = secsSince1900 - seventyYears;
      // print Unix time:
      Serial.println(epoch);

      // print the hour, minute and second:
      Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
      Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
      Serial.print(':');
      if ( ((epoch % 3600) / 60) < 10 ) {
        // In the first 10 minutes of each hour, we'll want a leading '0'
        Serial.print('0');
      }
      Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
      Serial.print(':');
      if ( (epoch % 60) < 10 ) {
        // In the first 10 seconds of each minute, we'll want a leading '0'
        Serial.print('0');
      }
      Serial.println(epoch % 60); // print the second
    }
  } else {
      Serial.println("Can not set time from the network since we are in soft AP mode.");     
  }
}

