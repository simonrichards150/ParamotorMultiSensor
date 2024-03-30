#include "include/Init.h"
#include "include/Power.h"
#include "include/GPS.h"
#include "include/Display.h"
#include "include/Temperature.h"
#include "include/Tach.h"
#include "include/MicroSD.h"
#include "include/LEDs.h"
#include "OneButton.h"

PowerHandler PWR = PowerHandler();
DisplayHandler GUI = DisplayHandler();
GPSHandler GPS = GPSHandler();
TempHandler TEMP = TempHandler();
TachHandler TACH = TachHandler();
MicroSDHandler SD = MicroSDHandler();
LEDHandler LEDS = LEDHandler();


void setup() {
  pinSetup();
  PWR.begin();
  GUI.begin();
  GUI.loadMainView();
  commsSetup();
  LEDS.begin();
  TEMP.begin(); 
  TACH.begin();
  SD.begin();
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

  if (digitalRead(BTN1) == HIGH)
  {
    SD.toggleLogging();
    delay(1000);
  }
  
  //Get new GPS position etc
  GPS.tick(0); 

  //Refresh the GUI
  GUI.update(TEMP.getCompTemp(),TACH.getRPM(), GPS.fix, GPS.sats.toInt(), GPS.epochTime(), GPS.hdg.toInt(), PWR.getBatteryMillivolts(), PWR.isCharging(), SD.getStatus()); 

  //Update MicroSD handler with new values
  SD.tick(GPS.epochTime(), GPS.fix, GPS.lat, GPS.latNS, GPS.lon, GPS.lonEW, GPS.hdg.toInt(), GPS.spd.toInt(), GPS.alt.toInt(), GPS.sats.toInt(), GPS.hdop.toDouble(), GPS.vdop.toDouble(), GPS.pdop.toDouble(), TACH.getRPM(), TEMP.getCompTemp(), PWR.getBatteryMillivolts());

  //Refresh the LEDs
  LEDS.tick(TEMP.getCompTemp());

  //delay(500); 
}
