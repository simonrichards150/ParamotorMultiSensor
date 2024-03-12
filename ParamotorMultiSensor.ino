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
  Serial.println(GPS.epochTime());
  GPS.printVars();

  //char val[256] = "$GNGGA,045712.800,5129.08142,N,00001.36838,W,1,13,1.2,10.5,M,45.6,M,,*68\n\n";
  //GPS.validNMEASentence(val);

tft.fillScreen(TFT_ORANGE);
  delay(10);
  tft.fillScreen(TFT_RED);
  delay(10);

}