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
	//Set up the internal ADC for battery voltage measurement
	analogReadResolution(12); 
	analogSetAttenuation(ADC_11db);
}

int PowerHandler::getBatteryMillivolts()
{
	return (int)round((double)analogReadMilliVolts(VBAT) * 1.5); //ADC reading is multiplied by 1.5 due to potential divider
}

bool PowerHandler::isCharging()
{
	if (digitalRead(CHG) == LOW) //Charging
	{
		return true;
	}
	else if (digitalRead(CHG) == HIGH) //Not charging
	{
		return false;
	}
}

void PowerHandler::sleep()
{
	esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL); //Disable all wakeup sources just in case
	esp_sleep_enable_ext1_wakeup(0x06, ESP_EXT1_WAKEUP_ANY_HIGH); //Enable wakeup on press of either BTN1 or BTN2
	esp_deep_sleep_start(); //Go to deep sleep
}



#endif