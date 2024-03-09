#include "include/Init.h"
#include "include/GPS.h"

GPSHandler GPS; 

void setup()
{
  pinSetup();
  commsSetup();
  GPS.begin();
  GPS.configure();
}

void loop()
{
  GPS.tick(1); //Must call tick() as often as possible. Setting to 1 will send GPS data to the PC serial, setting to 0 will stop the GPS data being sent to the PC serial
  delay(100); //Not necessary - only for testing the UART buffers
}