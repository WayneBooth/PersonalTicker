    
/* ----------------------------------- */
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 12
#define PAUSE_TIME 800
#define SCROLL_SPEED 20
#define CS_PIN PA4
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

/* ----------------------------------- */

#define _ESPLOGLEVEL_ 3
#include "WiFiEsp.h"
#include "secrets.h"
const char* host = "script.google.com";

/* ----------------------------------- */
#include <MSFDecoder.h>
#define MSF_PIN 21
MSFDecoder MSF;

/* ----------------------------------- */
#define LOCALDEBUG 1
#define WEBDATASIZE 96
char * databuffer[WEBDATASIZE];
char currentTime[6] = "--:--";


void setup()
{
  P.begin();
  printString("Starting...");

  MSF.init( MSF_PIN );
  while( ! MSF.getHasCarrier() ) {}
  printString(F("Carrier Found"));
  
  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network
  debugPrint(F(""));
  debugPrint(F("Connecting to WIFI"));
  debugPrint( ESP_SSID );

  // initialize serial for ESP module
  Serial1.begin(9600);
  WiFi.init(&Serial1);
  WiFi.begin( ESP_SSID, ESP_PASS );

  while (WiFi.status() != WL_CONNECTED) {
    debugPrint(F("WiFi connecting..."));
    delay(500);
  }

  debugPrint(F(""));
  debugPrint(F("WiFi connected"));
  debugPrint(F("IP address: "));
  debugPrint((String)WiFi.localIP());

  printString(currentTime);
}

void loop()
{
  static int pageFetchCounter = 1;
  
  if( --pageFetchCounter <= 0 ) {
    printString(F("Collecting Data"));
    getPage( host, PAGE );
    pageFetchCounter = 4;
  }

  Serial.println("Decoding data");
  for ( int a = 0 ; a < WEBDATASIZE ; a=a+2 ) {

    if( MSF.m_bHasValidTime ) {
      debugPrint(F("HAS VALID TIME"));
      sprintf(currentTime, "%02d:%02d", MSF.m_iHour, MSF.m_iMinute);
      MSF.m_bHasValidTime = false;
    }
  
    debugPrint(F("Parsing format : "));
    debugPrint((String)a);
    debugPrint(databuffer[a]);
    
    textEffect_t effect = PA_SCROLL_LEFT;
    if( strcmp(databuffer[a], "@@@drop") == 0 ) {
      effect = PA_OPENING_CURSOR;
      debugPrint(F("It's a DROP"));
    }
    else if( strcmp(databuffer[a], "@@@scroll") == 0 ) {
      effect = PA_SCROLL_LEFT;
      debugPrint(F("It's a SCROLL"));
    }

    debugPrint(F("Parsing line : "));
    debugPrint((String)a);
    debugPrint(databuffer[a+1]);
    
    if( databuffer[a+1][0] == '#' ) {
      debugPrint(F("Current Time"));
      printString( currentTime, effect, effect );
    }
    else {
      debugPrint(F("Direct Data"));
      printString( databuffer[a+1], effect, effect );
    }

    if( pageFetchCounter == 1 ) {
      free( databuffer[a] );
      free( databuffer[a+1] );
    }
    
  }

}

void debugPrint( String text ) {
  int x = 0;
#if LOCALDEBUG
  Serial.println( text );
#endif
}

void debugPrint( char * text ) {
  int x = 0;
#if LOCALDEBUG
  Serial.println( freeRam() );
  Serial.println( text );
#endif
}

void printString( char * text ) {
  printString( text, PA_OPENING, PA_NO_EFFECT );
}

void printString( char * text, textEffect_t start, textEffect_t stop ) {
  P.displayText(text, PA_CENTER, SCROLL_SPEED, PAUSE_TIME, start, stop);
  while ( ! P.displayAnimate() ) { }
}

void printString( String text ) {
  char * s = (char *)malloc( sizeof(char) * (text.length()+1) );
  text.toCharArray(s, (text.length()+1) );
  P.displayText(s, PA_CENTER, SCROLL_SPEED, PAUSE_TIME, PA_SCROLL_LEFT, PA_NO_EFFECT);
  while ( ! P.displayAnimate() ) { }
  free(s);
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

    int counter = 0;
    debugPrint(F("connecting to "));
    debugPrint(host);

    // Use WiFiClient class to create TCP connections
    WiFiEspClient client;
    const int httpPort = 443;

    if (!client.connectSSL(host, httpPort)) {
        debugPrint(F("connection failed"));
    }

    debugPrint(F("Requesting URL: "));
    debugPrint(String("GET ") + url + " HTTP/1.1");

    // This will send the request to the server
    client.print(String("GET ") + url + " HTTP/1.1\r\n");
    client.print(String("Host: ") + host + "\r\n" );
    client.print("Connection: close\r\n");
    client.println("");

    unsigned long timeout = millis();

    // Wait for response
    while (client.available() == 0) {
        if (millis() - timeout > 25000) {
            debugPrint(F(">>> Client Timeout !"));
            client.stop();
            return;
        }
    }

    debugPrint(F("Ready to read response."));

 /* while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }
  */
    // Read Headers
    bool lookForNewLocation = false;
    String header;
    while(client.available()) {
      header = client.readStringUntil('\n');
      if( header.startsWith("HTTP/1.1 302" ) ) {
        lookForNewLocation = true;
      }
      Serial.println(header);
      if( lookForNewLocation && header.startsWith("Location: " ) ) {
        break;
      }
      if (header == "\r") {
        debugPrint(F("headers received"));
        break;
      }
    }

    if( lookForNewLocation ) {

      debugPrint(F("Reading off remainder"));
      while (client.available()) { client.readStringUntil('\n'); }
      client.stop();
            
      debugPrint(F("Parsing new location"));
      header = header.substring( 18 ); // Everything after the "Location: "
      char * host1 = getHost(header);
      char * url1 = getUrl(header);
      debugPrint(F("Redirecting to " ));
      debugPrint(host1);
      debugPrint(url1);
      debugPrint(F(""));
      getPage( host1, url1 );
      free(host1);
      free(url1);
    }
    else {
      //client.readStringUntil('\n');
      // if there are incoming bytes available
      // from the server, read them and print them:
      String data = "";
      while (client.available()) {
        String s = client.readStringUntil('\n');
        debugPrint(F("Read Line : "));
        debugPrint(s);
        char * b = (char *)malloc( sizeof(char) * (s.length()+1) );
        s.toCharArray(b, (s.length()+1) );
        utf8Ascii(b);

        replaceAll(b, 176, 247);
        replaceAll(b, 163, 156);
        
        debugPrint(b);
        databuffer[counter++] = b;
      }
      debugPrint(F("Got Body Data"));
    }
    
    debugPrint(F(""));
    debugPrint(F("closing connection"));
  
}

void replaceAll(char * str, char oldChar, char newChar)
{
    int i = 0;

    // Run till end of string 
    while(str[i] != '\0')
    {
        // If occurrence of character is found
        if(str[i] == oldChar)
        {
            str[i] = newChar;
        }

        i++;
    }
}

uint8_t utf8Ascii(uint8_t ascii)
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
{
  static uint8_t cPrev;
  uint8_t c = '\0';

  if (ascii < 0x7f)   // Standard ASCII-set 0..0x7F, no conversion
  {
    cPrev = '\0';
    c = ascii;
  }
  else
  {
    switch (cPrev)  // Conversion depending on preceding UTF8-character
    {
    case 0xC2: c = ascii;  break;
    case 0xC3: c = ascii | 0xC0;  break;
    case 0x82: if (ascii==0xAC) c = 0x80; // Euro symbol special case
    }
    cPrev = ascii;   // save last char
  }

  //PRINTX("\nConverted 0x", ascii);
  //PRINTX(" to 0x", c);

  return(c);
}

void utf8Ascii(char* s)
// In place conversion UTF-8 string to Extended ASCII
// The extended ASCII string is always shorter.
{
  uint8_t c, k = 0;
  char *cp = s;

  //PRINT("\nConverting: ", s);

  while (*s != '\0')
  {
    c = utf8Ascii(*s++);
    if (c != '\0')
      *cp++ = c;
  }
  *cp = '\0';   // terminate the new string
}

int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

