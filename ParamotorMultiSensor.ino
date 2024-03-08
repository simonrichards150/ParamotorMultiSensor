#include "include/Init.h"

void setup()
{
  pinSetup();
  commsSetup();
}

void loop()
{
Serial.println("Hello World!");
delay(1000);

}