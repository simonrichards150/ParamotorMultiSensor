#include "include/Init.h"
#include "include/GPS.h"
#include "include/Display.h"

TFT_eSPI tft = TFT_eSPI();
DisplayHandler GUI = DisplayHandler(&tft);
GPSHandler GPS = GPSHandler();

void setup() {
  // put your setup code here, to run once:
  pinSetup();
  commsSetup();
  GUI.begin();
  GPS.begin();
  GPS.configure();
}

void loop() {
  GPS.tick(0);
  //Serial.println(GPS.epochTime());
  //GPS.printVars();
tft.fillScreen(TFT_ORANGE);
  delay(1000);
  tft.fillScreen(TFT_RED);
  delay(1000);

}