

/* ----------------------------------- */

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "secrets.h"
const char* host = "script.google.com";

/* ----------------------------------- */

#define LOCALDEBUG 0
#define WEBDATASIZE 96
char * databuffer[WEBDATASIZE];

void debugPrint( String text ) {
  int x = 0;
#if LOCALDEBUG
  Serial.println( text );
#endif
}


void setup() {
  
  Serial.begin(9600);
#if LOCALDEBUG
  Serial.setDebugOutput(true);
#else
  Serial.setDebugOutput(false);
#endif

  // initialize serial for ESP module
  WiFi.softAPdisconnect();
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  delay(100);
  WiFi.begin( ESP_SSID, ESP_PASS );

  while (WiFi.status() != WL_CONNECTED) {
    debugPrint(".");
    delay(500);
  }
  
}

void loop() {
  
  static int pageFetchCounter = 1;
  
  if( --pageFetchCounter <= 0 ) {
    getPage( host, PAGE );
    pageFetchCounter = 4;
  }

  for ( int a = 0 ; a < WEBDATASIZE ; a++ ) {

    while( ! Serial.available() ) {
      // Hangout waiting for the client to ask for info
      delay(5);
    }
    debugPrint("Got request for data");
    Serial.readStringUntil('\n');
    Serial.print(databuffer[a]);
    Serial.print("\n");
    
    if( pageFetchCounter == 1 ) {
      free( databuffer[a] );
    }
    
  }

}

char * getHost( String header) {
  String host = header.substring( 0, header.indexOf("/") );
  char * s = (char *)malloc( sizeof(char) * (host.length()+1) );
  host.toCharArray(s, (host.length()+1) );
  return s;
}

char * getUrl( String header) {
  String url = header.substring( header.indexOf("/") );
  char * s = (char *)malloc( sizeof(char) * (url.length()+1) );
  url.toCharArray(s, (url.length()+1) );
  return s;
}

void getPage( const char * host, const char * url ) {

    String propocol = "HTTP/1.0";
    int counter = 0;
    debugPrint("Getting page: ");
    debugPrint(url);

    // Use WiFiClient class to create TCP connections
    WiFiClientSecure client;
    const int httpPort = 443;

    if (!client.connect(host, httpPort)) {
        return;
    }

    // This will send the request to the server
    client.print(String("GET ") + url + " " + propocol + "\r\n");
    client.print(String("Host: ") + host + "\r\n" );
    client.print("Connection: close\r\n");
    client.println("");

    unsigned long timeout = millis();

    // Wait for response
    while (client.available() == 0) {
        if (millis() - timeout > 25000) {
            client.stop();
            return;
        }
    }
  
    // Read Headers
    bool lookForNewLocation = false;
    String header;
    while(client.available()) {
      header = client.readStringUntil('\n');
      if( header.startsWith(propocol + " 302" ) ) {
        lookForNewLocation = true;
      }

      if( lookForNewLocation && header.startsWith("Location: " ) ) {
        break;
      }
      if (header == "\r") {
        break;
      }
    }

    if( lookForNewLocation ) {

      while (client.available()) { client.readStringUntil('\n'); }
      client.stop();

      header = header.substring( 18 ); // Everything after the "Location: "
      char * host1 = getHost(header);
      char * url1 = getUrl(header);

      getPage( host1, url1 );
      free(host1);
      free(url1);
    }
    else {

      String data = "";
      while (client.available()) {
        String s = client.readStringUntil('\n');
        
        char * b = (char *)malloc( sizeof(char) * (s.length()+1) );
        s.toCharArray(b, (s.length()+1) );
        utf8Ascii(b);

        replaceAll(b, 176, 247);
        replaceAll(b, 163, 156);
        
        databuffer[counter++] = b;
      }
    }
  
}

void replaceAll(char * str, char oldChar, char newChar) {
    int i = 0;

    // Run till end of string 
    while(str[i] != '\0') {
        // If occurrence of character is found
        if(str[i] == oldChar) {
            str[i] = newChar;
        }

        i++;
    }
}


// Convert a single Character from UTF8 to Extended ASCII according to ISO 8859-1,
// also called ISO Latin-1. Codes 128-159 contain the Microsoft Windows Latin-1
// extended characters:
// - codes 0..127 are identical in ASCII and UTF-8
// - codes 160..191 in ISO-8859-1 and Windows-1252 are two-byte characters in UTF-8
//                 + 0xC2 then second byte identical to the extended ASCII code.
// - codes 192..255 in ISO-8859-1 and Windows-1252 are two-byte characters in UTF-8
//                 + 0xC3 then second byte differs only in the first two bits to extended ASCII code.
// - codes 128..159 in Windows-1252 are different, but usually only the €-symbol will be needed from this range.
//                 + The euro symbol is 0x80 in Windows-1252, 0xa4 in ISO-8859-15, and 0xe2 0x82 0xac in UTF-8.
//
// Modified from original code at http://playground.arduino.cc/Main/Utf8ascii
// Extended ASCII encoding should match the characters at http://www.ascii-code.com/
//
// Return "0" if a byte has to be ignored.
uint8_t utf8Ascii(uint8_t ascii) {
  
  static uint8_t cPrev;
  uint8_t c = '\0';

  if (ascii < 0x7f) {  // Standard ASCII-set 0..0x7F, no conversion
    cPrev = '\0';
    c = ascii;
  }
  
  else {
    switch (cPrev) { // Conversion depending on preceding UTF8-character
      case 0xC2: 
        c = ascii;  
        break;
        
      case 0xC3: 
        c = ascii | 0xC0;
        break;
        
      case 0x82: 
        if (ascii==0xAC) c = 0x80; // Euro symbol special case
    }
    cPrev = ascii;   // save last char
  }

  return(c);
}


// In place conversion UTF-8 string to Extended ASCII
// The extended ASCII string is always shorter.
void utf8Ascii(char* s) {
  uint8_t c, k = 0;
  char *cp = s;

  //PRINT("\nConverting: ", s);

  while (*s != '\0') {
    c = utf8Ascii(*s++);
    if (c != '\0')
      *cp++ = c;
  }
  *cp = '\0';   // terminate the new string
  
}

