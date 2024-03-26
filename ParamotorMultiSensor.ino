#include "include/Init.h"
#include "include/GPS.h"
#include "include/Display.h"
#include "include/Temperature.h"
#include "include/Tach.h"

DisplayHandler GUI = DisplayHandler();
GPSHandler GPS = GPSHandler();
TempHandler TEMP = TempHandler();
TachHandler TACH = TachHandler();

int bat = 1000;
int dir = 0;
bool islogging = false;

void setup() {
  pinSetup();
  GUI.begin();
  GUI.splash();
  commsSetup();
  TACH.begin();
  GPS.begin();
  GPS.configure();
  GUI.loadMainView();
  TEMP.begin(); //Uncomment once temperature handler is done <-----------------
  

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


  GUI.update(TEMP.getCompTemp(),TACH.getRPM(),0,"Hi", 0, bat, false, "Hola", islogging); //Fake values, only bat is actually used for anything

  if (dir == 0)
    bat-=10;

  if (dir == 1)
    bat+=10;

  if (bat < 10)
  {
    dir = 1;
    islogging = true;
  }

  if (bat > 1000)
  {
    dir = 0;
    islogging = false;
  }
  

  
  //delay(500); 
}
