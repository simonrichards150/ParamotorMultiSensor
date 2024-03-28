#include "include/Init.h"
#include "include/Power.h"
#include "include/GPS.h"
#include "include/Display.h"
#include "include/Temperature.h"
#include "include/Tach.h"
#include "include/MicroSD.h"

PowerHandler PWR = PowerHandler();
DisplayHandler GUI = DisplayHandler();
GPSHandler GPS = GPSHandler();
TempHandler TEMP = TempHandler();
TachHandler TACH = TachHandler();
MicroSDHandler SD = MicroSDHandler();


void setup() {
  pinSetup();
  PWR.begin();
  GUI.begin();
  GUI.loadMainView();
  commsSetup();
  TEMP.begin(); 
  TACH.begin();
  SD.begin();
  GPS.begin();
  GPS.configure();
  
 /*
 digitalWrite(LED1, HIGH);
 digitalWrite(LED2, LOW);
 digitalWrite(LED3, LOW);
 analogWrite(LEDR, 25);
 analogWrite(LEDG, 25);
 analogWrite(LEDB, 25);
 */


  /*Serial.println("Rebooted. Press button");
  
  while(!digitalRead(BTN1)) //In case of crash this will halt the program after reboot
  {
    ;
  }
  digitalWrite(TACH_EN, HIGH);*/
}

void loop() {

  if (digitalRead(BTN1) == HIGH)
  {
    SD.startLogging();
    delay(2000);
  }
  else if (digitalRead(BTN2) == HIGH)
  {
    SD.stopLogging();
    delay(2000);
  }
  
  GPS.tick(0); //Update GPS
  //GPS.printVars(); //Print GPS information

//CHANGE BACK TO COMPENSATED TEMPERATURE
  GUI.update(TEMP.getCJT(),TACH.getRPM(), GPS.fix, GPS.sats.toInt(), GPS.epochTime(), GPS.hdg.toInt(), PWR.getBatteryMillivolts(), PWR.isCharging(), SD.getStatus()); 

//CHANGE BACK TO COMPENSATED TEMPERATURE
  SD.tick(GPS.epochTime(), GPS.fix, GPS.lat, GPS.latNS, GPS.lon, GPS.lonEW, GPS.hdg.toInt(), GPS.spd.toInt(), GPS.alt.toInt(), GPS.sats.toInt(), GPS.hdop.toDouble(), GPS.vdop.toDouble(), GPS.pdop.toDouble(), TACH.getRPM(), TEMP.getCJT(), PWR.getBatteryMillivolts());


  //delay(500); 
}
