    
/* ----------------------------------- */
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 12
#define PAUSE_TIME 800
#define SCROLL_SPEED 20
#define CS_PIN 10
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

/* ----------------------------------- */
#include <MSFDecoder.h>
#define MSF_PIN 3
MSFDecoder MSF;

/* ----------------------------------- */
#include <SoftwareSerial.h>
#define DATA_RX 6
#define DATA_TX 5
SoftwareSerial dataSerial(DATA_RX, DATA_TX);

/* ----------------------------------- */
char currentTime[6] = "--:--";
const byte numChars = 1000;
char data[numChars];

void setup() {

  // Setup Serial debugging
  //Serial.begin(9600);
  //delay(10);

  // Setup display
  P.begin();
  delay(10);
  printString("Starting...");

  // Setup WWVB receiver
  MSF.init( MSF_PIN );
  while( ! MSF.getHasCarrier() ) {}

  // Open comms to ESP8266
  dataSerial.begin(9600);

  printString("Collecting Data");
}

void loop() {

  static textEffect_t effect = PA_SCROLL_LEFT;

  // Ask ESP8266 for data
  dataSerial.println();
  while(! dataSerial.available() ) {
    delay(50);
  }
  readLine();
  //debugPrint( data );


  if( strcmp(data, "@@@drop") == 0 ) {
    effect = PA_OPENING_CURSOR;
    //debugPrint(F("It's a DROP"));
  }
  else if( strcmp(data, "@@@scroll") == 0 ) {
    effect = PA_SCROLL_LEFT;
    //debugPrint(F("It's a SCROLL"));
  }
  else if( data[0] == '#' ) {
    if( MSF.m_bHasValidTime ) {
      //debugPrint(F("HAS VALID TIME"));
      sprintf(currentTime, "%02d:%02d", MSF.m_iHour, MSF.m_iMinute);
      MSF.m_bHasValidTime = false;
    }
    printString( currentTime, effect, effect );
  }
  else {
    printString( data, effect, effect );
  }
  //free( data );

}

void readLine() {

  static byte ndx = 0;
  char rc;

  while ( 1 ) {
    while(! dataSerial.available() ) {
      delay(5);
    }
    rc = dataSerial.read();
    if (rc != '\n') {
      data[ndx] = rc;
      ndx++;
      if (ndx >= numChars) {
        ndx = numChars - 1;
      }
    }

    else {
      data[ndx] = '\0'; // terminate the string
      ndx = 0;
      break;
    }
  }
}

void debugPrint( String text ) {
  Serial.println( text );
}

void debugPrint( char * text ) {
  Serial.println( text );
}

void printString( char * text ) {
  printString( text, PA_SCROLL_LEFT, PA_NO_EFFECT );
}

void printString( char * text, textEffect_t start, textEffect_t stop ) {
  P.displayText(text, PA_CENTER, SCROLL_SPEED, PAUSE_TIME, start, stop);
  while ( ! P.displayAnimate() ) { }
}

/*
void printString( String text ) {
  char *s = StringToChars( text );
  P.displayText(s, PA_CENTER, SCROLL_SPEED, PAUSE_TIME, PA_SCROLL_LEFT, PA_NO_EFFECT);
  while ( ! P.displayAnimate() ) { }
  free(s);
}

char *StringToChars( String text ) {
  char * s = (char *)malloc( sizeof(char) * (text.length()+1) );
  text.toCharArray(s, (text.length()+1) );
  return s;
}
*/
