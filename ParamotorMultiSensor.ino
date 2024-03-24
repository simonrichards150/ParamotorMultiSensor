#include "include/Init.h"
#include "include/GPS.h"
#include "include/Display.h"
#include "include/Temperature.h"
#include "include/Tach.h"

DisplayHandler GUI = DisplayHandler();
GPSHandler GPS = GPSHandler();
TempHandler TEMP = TempHandler();
TachHandler TACH = TachHandler();

int bat = 4200;
int dir = 0;

void setup() {
  pinSetup();
  GUI.begin();
  GUI.splash();
  commsSetup();
  TACH.begin();
  GPS.begin();
  GPS.configure();
  GUI.loadMainView();
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
  //Serial.println(TACH.getRPM());

  GUI.update(0,0,0,"Hi", bat, false, "Hola"); //Fake values, only bat is actually used for anything

  if (dir == 0)
    bat--;

  if (dir == 1)
    bat++;

  if (bat < 3100)
    dir = 1;

  if (bat > 4200)
    dir = 0;
  

  
  delay(3); 
}
