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

OneButton LogButton(BTN1, false, false);
OneButton LEDButton(BTN2, false, false);
OneButton PowerButton(BTN3, false, false);

void IRAM_ATTR checkButtonTicks() //Buttons ISR
{
  LogButton.tick(); 
  LEDButton.tick();
  PowerButton.tick();
}

//Button functions
void toggleLog()
{
  SD.toggleLogging();
}

void toggleLEDs()
{
  LEDS.toggleEnable();
}

void sleep()
{
//Add sleep functions to peripherals and put here
  SD.sleep();
  TEMP.sleep();
  TACH.sleep();
  LEDS.sleep();
  GUI.sleep();
  GPS.sleep();
  PWR.sleep();
}

void setup() {
  pinSetup();
  PWR.begin();
  GUI.begin();
  commsSetup();
  LEDS.begin();
  TEMP.begin(); 
  TACH.begin();
  SD.begin();
  GPS.begin();
  GPS.configure();

  //Configure button interrupts
  attachInterrupt(digitalPinToInterrupt(BTN1), checkButtonTicks, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BTN2), checkButtonTicks, CHANGE);
  attachInterrupt(digitalPinToInterrupt(BTN3), checkButtonTicks, CHANGE);

  //Configure button functions
  LogButton.attachClick(toggleLog);
  LEDButton.attachClick(toggleLEDs);
  PowerButton.setPressMs(2000);
  PowerButton.attachLongPressStart(sleep);
}

void loop() {

  checkButtonTicks();
  
  //Get new GPS position etc
  GPS.tick(0); 

  //Refresh the GUI
  GUI.update(TEMP.getCompTemp(),TACH.getRPM(), GPS.fix, GPS.sats.toInt(), GPS.epochTime(), GPS.hdg.toInt(), PWR.getBatteryMillivolts(), PWR.isCharging(), SD.getStatus()); 

  //Update MicroSD handler with new values
  SD.tick(GPS.epochTime(), GPS.fix, GPS.lat, GPS.latNS, GPS.lon, GPS.lonEW, GPS.hdg.toInt(), GPS.spd.toInt(), GPS.alt.toInt(), GPS.sats.toInt(), GPS.hdop.toDouble(), GPS.vdop.toDouble(), GPS.pdop.toDouble(), TACH.getRPM(), TEMP.getCompTemp(), PWR.getBatteryMillivolts(), LEDS.getLEDState());

  //Refresh the LEDs
  LEDS.tick(TEMP.getCompTemp());

  //delay(500); //For debug only
}
