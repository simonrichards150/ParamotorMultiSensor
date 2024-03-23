#include "include/Init.h"
#include "include/GPS.h"
#include "include/Display.h"
#include "include/Temperature.h"
#include "include/Tach.h"

DisplayHandler GUI = DisplayHandler();
GPSHandler GPS = GPSHandler();
TempHandler TEMP = TempHandler();
TachHandler TACH = TachHandler();


void setup() {
  pinSetup();
  GUI.begin();
  GUI.splash();
  commsSetup();
  TACH.begin();
  GPS.begin();
  GPS.configure();

  //TEMP.begin(); //Uncomment once temperature handler is done <-----------------
  

  /*Serial.println("Rebooted. Press button");
  
  while(!digitalRead(BTN1)) //In case of crash this will halt the program after reboot
  {
    ;
  }
  digitalWrite(TACH_EN, HIGH);*/
}

void loop() {
  
  GPS.tick(0); //Update GPS
  //GPS.printVars(); //Print GPS information

  //Serial.println(TEMP.getCJT()); //Uncomment once temperature handler is done <----------------
  //Serial.println(TEMP.getUnCompTemp());
  //Serial.println(TEMP.getCompTemp());
  Serial.println(TACH.getRPM());
  
  delay(100); //1 second delay
}
