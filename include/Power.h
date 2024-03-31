//Power Management Handler
//Mozes Baiden

#ifndef POWER_H
#define POWER_H

#include <Arduino.h>

class PowerHandler
{
public:
	PowerHandler();
	void begin();
	int getBatteryMillivolts();
	bool isCharging();
	void sleep();

private:
	
};

PowerHandler::PowerHandler()
{
	//Nothing
}

void PowerHandler::begin()
{
	analogReadResolution(12);
	analogSetAttenuation(ADC_11db);
}

int PowerHandler::getBatteryMillivolts()
{
	return (int)round((double)analogReadMilliVolts(VBAT) * 1.5);
}

bool PowerHandler::isCharging()
{
	if (digitalRead(CHG) == LOW)
	{
		return true;
	}
	else if (digitalRead(CHG) == HIGH)
	{
		return false;
	}
}

void PowerHandler::sleep()
{
	
}



#endif