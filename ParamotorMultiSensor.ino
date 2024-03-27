#include "include/Init.h"
#include "include/Power.h"
#include "include/GPS.h"
#include "include/Display.h"
#include "include/Temperature.h"
#include "include/Tach.h"

PowerHandler PWR = PowerHandler();
DisplayHandler GUI = DisplayHandler();
GPSHandler GPS = GPSHandler();
TempHandler TEMP = TempHandler();
TachHandler TACH = TachHandler();

int bat = 1000;
int dir = 0;
bool islogging = false;

void setup() {
  pinSetup();
  PWR.begin();
  GUI.begin();
  GUI.loadMainView();
  commsSetup();
  TEMP.begin(); 
  TACH.begin();
  GPS.begin();
  GPS.configure();
  
 

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

  //Serial.println(GPS.fix);
  //Serial.println(GPS.sats.toInt());
  //Serial.println(GPS.epochTime());
  //Serial.println(GPS.hdg.toDouble());
  
  GUI.update(TEMP.getCompTemp(),TACH.getRPM(), GPS.fix, GPS.sats.toInt(), GPS.epochTime(), GPS.hdg.toInt(), PWR.getBatteryMillivolts(), PWR.isCharging(), islogging); 

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
