#include "include/Init.h"
#include "include/GPS.h"
#include "include/Display.h"


DisplayHandler GUI = DisplayHandler();
GPSHandler GPS = GPSHandler();

void setup() {
  // put your setup code here, to run once:
  pinSetup();
  GUI.begin();
  GUI.splash();
  commsSetup();
  GPS.begin();
  GPS.configure();
 delay(1000);
  Serial.println("Press button");
  while(!digitalRead(BTN1))
  {
    ;
  }
}

void loop() {
  
  GPS.tick(0);
  //Serial.println(GPS.epochTime());
  GPS.printVars();
  Serial.printf("\nFree: %d\n", ESP.getFreeHeap());
  Serial.printf("MaxAlloc: %d\n\n", ESP.getMaxAllocHeap());

  delay(500);
}
