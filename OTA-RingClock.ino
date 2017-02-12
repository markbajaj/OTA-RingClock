// Credit for initial code https://www.hackster.io/thearduinoguy/esp8266-neopixel-ring-clock-a9cc74
// Credit Mike McRoberts https://www.hackster.io/mike-mcroberts
// I added the Wifi Manager and OTA updates through Arduino IDE

#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>  
#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include "TimeClient.h"

//Define the ESP8266 PIN used for data on the ring
#define PIN D1 
long lastUpdate = millis();
long lastSecond = millis();

String hours, minutes, seconds;
int currentSecond, currentMinute, currentHour;

//Number of LEDs in the ring
int ledsInRing = 12;

const float UTC_OFFSET = 0;
TimeClient timeClient(UTC_OFFSET);


Adafruit_NeoPixel strip = Adafruit_NeoPixel(12, PIN);

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());

  Serial.println(myWiFiManager->getConfigPortalSSID());
}


void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  strip.begin();
  strip.setBrightness(128);
  strip.show();

  // ***************************************************************************
  // Setup: WiFiManager
  // ***************************************************************************
  WiFiManager wifiManager;
  wifiManager.setAPCallback(configModeCallback);
  wifiManager.autoConnect();


  digitalWrite(BUILTIN_LED, LOW);

  timeClient.updateTime();
  updateTime() ;
  lastUpdate = millis();
  lastSecond = millis();

// OTA Bit *****************************************
//  Serial.begin(115200);
//  Serial.println("Booting");
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("ESP8266-RING");

  // No authentication by default uncomment below and change password
  //ArduinoOTA.setPassword((const char *)"password");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

// OTA End **************************************************
  
}

void loop()
{

// OTA Bit ************************
  ArduinoOTA.handle();
// End OTA ************************ 
  
  if ((millis() - lastUpdate) > 1800000) updateTime();

  if ((millis() - lastSecond) > 1000)
  {

    strip.setPixelColor(currentSecond / 5, 0, 0, 0);
    strip.setPixelColor(currentMinute / 5, 0, 0, 0);
    strip.setPixelColor(currentHour, 0, 0, 0);
    
//    strip.setPixelColor(currentSecond * ( ledsInRing / 60), 0, 0, 0);
//    strip.setPixelColor(currentMinute * ( ledsInRing / 60), 0, 0, 0);
//    strip.setPixelColor(currentHour * ( ledsInRing / 12), 0, 0, 0);
    
    strip.show();
    lastSecond = millis();
    currentSecond++;
    if (currentSecond > 59)
    { currentSecond = 0;
      currentMinute++;
      if (currentMinute > 59) {
        currentMinute = 0;
        currentHour++;
      if (currentHour > 12) currentHour = 0;
      }
    }
    String currentTime = String(currentHour) + ':' + String(currentMinute) + ':' + String(currentSecond);
    Serial.println(currentTime);


    strip.setPixelColor(currentSecond / 5, 0, 0, 255);
    strip.setPixelColor(currentMinute / 5, 0, 255, 0);
    strip.setPixelColor(currentHour, 255, 0, 0);
    
//    strip.setPixelColor(currentSecond * ( ledsInRing / 60), 0, 0, 255);
//    strip.setPixelColor(currentMinute * ( ledsInRing / 60), 0, 255, 0);
//    strip.setPixelColor(currentHour * ( ledsInRing / 12), 255, 0, 0);
    strip.show();
  }
}

void updateTime() 
{
  hours = timeClient.getHours();
  minutes = timeClient.getMinutes();
  seconds = timeClient.getSeconds();
  currentHour = hours.toInt();
 // if (currentHour = 12) currentHour = 12;
  if (currentHour > 12) currentHour = currentHour - 12;
 // if (currentHour = 0) currentHour = 12;
  currentMinute = minutes.toInt();
  currentSecond = seconds.toInt();
  lastUpdate = millis();
}
