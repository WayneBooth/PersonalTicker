//define PAROLAOUT 1

#ifdef PAROLAOUT
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#endif

#include <Adafruit_ESP8266.h>
#include <SoftwareSerial.h>

#include <MSFDecoder.h>

#ifdef PAROLAOUT
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 8
#define PAUSE_TIME 500
#define SCROLL_SPEED 5
#define CS_PIN 14

MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
#endif

// Links to an ESP8266
#define ESP_RX   8
#define ESP_TX   9
#define ESP_RST  10

#include "secrets.h"
#define HOST     "script.google.com"     // Host to contact
#define PAGE     "/macros/s/AKfycbxpb_mqMl1YqnP54bmL-ABGb5cRC4Dab6b2a0Hz41GCgoaipR8O/exec" // Web page to request
#define PORT     80                     // 80 = HTTP default port

#define DATA   {"data":[{"Current Date":{"start":"swipe","transition":"drop","entries":[{"entry":"Sat Aug 25 2018"}]}},{"Time Data Collected":{"start":"swipe","transition":"drop","entries":[{"entry":"22:57:13 GMT+0100 (BST)"}]}},{"Family Calendar Events":{"start":"swipe","transition":"drop","entries":[{"entry":"Some holiday"},{"entry":"other event"},{"entry":"more stuff"}]}},{"Current Time":{"start":"swipe","transition":"drop","entries":[{"function":"getCurrentTime()"}]}},{"UK News":{"start":"swipe","transition":"drop","entries":[{"entry":"Papal visit: Pope shamed by Church's abuse failures : On a historic visit to Ireland, Pope Francis reportedly condemns abuse in the Church as \"filth\"."},{"entry":"Abuse scandal is 'source of pain' : Pope Francis and Taoiseach Leo Varadkar addressed the child sex abuse scandal in the Catholic church."},{"entry":"Egypt hotel deaths: 'No toxic gas in UK couple's room' : British holidaymakers have been flown home from a hotel in Egypt where a couple died."},{"entry":"Lindsay Kemp, performer and Bowie mentor, dies at 80 : He mentored David Bowie and taught dance to Kate Bush, who calls him a \"truly original and great artist\"."}]}},{"Current Time":{"start":"swipe","transition":"drop","entries":[{"function":"getCurrentTime()"}]}},{"Kirkcaldy Weather":{"start":"swipe","transition":"drop","entries":[{"entry":"Saturday: Partly Cloudy, Minimum Temperature: 6°C (42°F) : Minimum Temperature: 6°C (42°F), Wind Direction: South Westerly, Wind Speed: 11mph, Visibility: Very Good, Pressure: 1012mb, Humidity: 71%, UV Risk: 3, Pollution: Low, Sunset: 20:26 BST"},{"entry":"Sunday: Light Rain Showers, Minimum Temperature: 9°C (48°F) Maximum Temperature: 13°C (55°F) : Maximum Temperature: 13°C (55°F), Minimum Temperature: 9°C (48°F), Wind Direction: South Easterly, Wind Speed: 12mph, Visibility: Good, Pressure: 1003mb, Humidity: 91%, UV Risk: 1, Pollution: Low, Sunrise: 06:04 BST, Sunset: 20:24 BST"}]}},{"Current Time":{"start":"swipe","transition":"drop","entries":[{"function":"getCurrentTime()"}]}},{"Gold Price":{"start":"swipe","transition":"drop","entries":"£938.64"}}]}'

SoftwareSerial softser(ESP_RX, ESP_TX);
#ifndef PAROLAOUT
Adafruit_ESP8266 wifi(&softser, &Serial, ESP_RST);
#else
Adafruit_ESP8266 wifi(&softser, NULL, ESP_RST);
#endif

MSFDecoder MSF;

char buffer[50];

void setup() {

  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  
  wifi.setBootMarker(F("Version:0.9.2.4]\r\n\r\nready"));

  softser.begin(9600); // Soft serial connection to ESP8266
#ifndef PAROLAOUT
  Serial.begin(115200); while(!Serial); // UART serial debug
#else
  P.begin();
#endif

  MSF.init();
  printString(DATA);
  printString(F("Starting..."));

  while( ! MSF.getHasCarrier() ) {}
  printString(F("Carrier Found"));

  digitalWrite(4, LOW);

  printString(F("Init ESP8266"));
 
  printString(F("Hard Reset"));
  if(!wifi.hardReset()) {
    printString(F("Error on hard reset."));
    for(;;);
  }

  printString(F("Soft Reset"));
  if(!wifi.softReset()) {
    printString(F("Error on soft reset."));
    for(;;);
  }

  printString(F("Set Baud"));
  wifi.println(F("AT+UART_DEF=9600,8,1,0,0"));
  if(wifi.readLine(buffer, sizeof(buffer))) {
    printString(F("Response = "));
    printString(buffer);
    wifi.find(); // Discard the 'OK' that follows
  } else {
    printString(F("Baud: Error"));
  }

  printString(F("Checking firmware version..."));
  wifi.println(F("AT+GMR"));
  if(wifi.readLine(buffer, sizeof(buffer))) {
    printString(F("Response = "));
    printString(buffer);
    wifi.find(); // Discard the 'OK' that follows
  } else {
    printString(F("Ver: Error"));
  }

  printString(F("Checking MAC address..."));
  wifi.println(F("AT+CIPSTAMAC_DEF"));
  if(wifi.readLine(buffer, sizeof(buffer))) {
    printString(F("Response = "));
    printString(buffer);
    wifi.find(); // Discard the 'OK' that follows
  } else {
    printString(F("MAC: Error"));
  }

  printString(F("Connecting..."));
  if(wifi.connectToAP(F(ESP_SSID), F(ESP_PASS))) {

    // IP addr check isn't part of library yet, but
    // we can manually request and place in a string.
    printString(F("OK\nChecking IP addr..."));
    wifi.println(F("AT+CIFSR"));
    if(wifi.readLine(buffer, sizeof(buffer))) {
      printString(buffer);
      wifi.find(); // Discard the 'OK' that follows

      printString(F("Getting Page."));
      printString(F("Connecting to host..."));
      if(wifi.connectTCP(F(HOST), PORT)) {
        printString(F("OK\nRequesting page..."));
        if(wifi.requestURL(F(PAGE))) {
          printString(F("OK\nSearching for string..."));
          // Search for a phrase in the open stream.
          // Must be a flash-resident string (F()).
          if(wifi.find(F("\r\n\r\n"), true)) {
            printString(F("WORKING !!!"));
          } else {
            printString(F("Incorrect response"));
          }
        } else { // URL request failed
          printString(F("Page fetch fail"));
        }
        wifi.closeTCP();
      } else { // TCP connect failed
        printString(F("Connection Failed"));
      }
    } else { // IP addr check failed
      printString(F("No IP address"));
    }
    wifi.closeAP();
  } else { // WiFi connection failed
    printString(F("WIFI Failure"));
  }

}

bool g_bPrevCarrierState;
uint8_t g_iPrevBitCount;

void loop() {

  char *msg[] = { "Ambulance", "Emergency", "----------------", };
  static uint8_t cycle = 0;

  bool bCarrierState = MSF.getHasCarrier();
  uint8_t iBitCount = MSF.getBitCount();
  if ((bCarrierState != g_bPrevCarrierState) || (bCarrierState == true && iBitCount != g_iPrevBitCount))
  {
    sprintf(buffer, "%02d", iBitCount);
    printString(buffer);
  }
  g_bPrevCarrierState = bCarrierState;
  g_iPrevBitCount = iBitCount;
    
  if( MSF.m_bHasValidTime ) {
    digitalWrite(4, HIGH);
    sprintf(buffer, "%02d:%02d", MSF.m_iHour, MSF.m_iMinute);
    printString(buffer);
    msg[2] = buffer;
    MSF.m_bHasValidTime = false;
  }

  if ( P.displayAnimate() ) {
    // set up the string
    P.displayText(msg[cycle], PA_CENTER, SCROLL_SPEED, PAUSE_TIME, PA_GROW_UP, PA_RANDOM);

    // prepare for next pass
    cycle = (cycle + 1) % ARRAY_SIZE(msg);
  }

}

void printString( const __FlashStringHelper * text ) {
#ifdef PAROLAOUT
  P.displayText(text, PA_CENTER, SCROLL_SPEED, PAUSE_TIME, PA_OPENING, PA_GROW_UP);
  while ( ! P.displayAnimate() ) { }
#else
  Serial.println(text);
#endif
}

void printString( char * text ) {
#ifdef PAROLAOUT
  P.displayText(text, PA_CENTER, SCROLL_SPEED, PAUSE_TIME, PA_OPENING, PA_GROW_UP);
  while ( ! P.displayAnimate() ) { }
#else
  Serial.println(text);
#endif
}
