//Tachometer Handler
//Simon Richards-Martin

#ifndef TACH_H
#define TACH_H

#include <Arduino.h>
#include <soc/mcpwm_struct.h>
#include <driver/mcpwm.h>

#define TACH_RPM_MAX 50000
#define TACH_RPM_MIN 300
#define countArraySize 7
#define countsSinceLastPulseLimit 10

class TachHandler
{
public:
	TachHandler();
	void begin();
	int getRPM();
	void enable(int);
	void sleep();

private:
	long countArray[countArraySize] = {0};
	long countSorted[countArraySize] = {0};
	
	
};

//Begin global code section (ISR can't be inside class)

long TachHandler_counts = 0;
long TachHandler_prev = 0;
long TachHandler_Counts_Prev = 0;
long TachHandler_Cycles_Since_Last_Pulse = 0;

portMUX_TYPE timer_mux = portMUX_INITIALIZER_UNLOCKED;

void cap_event_handler(void *arg)
{
	portENTER_CRITICAL_ISR(&timer_mux); // Disable other interrupts temporarily
	TachHandler_counts = MCPWM0.cap_chn[0].val - TachHandler_prev; // Calculate number of cycles between last pulse n and pulse n-1 (12.5ns/cycle)
	
	if (TachHandler_counts < 320000) // Ignore pulses shorter than 4ms (12.5ns*320000=250Hz)
	{ 
		TachHandler_counts = TachHandler_Counts_Prev;
	}
	
	TachHandler_Counts_Prev = TachHandler_counts;
	TachHandler_prev = MCPWM0.cap_chn[0].val; 
	TachHandler_Cycles_Since_Last_Pulse = 0;
	MCPWM0.int_clr.val = BIT(27);
	portEXIT_CRITICAL_ISR(&timer_mux);
}

//End global code section

TachHandler::TachHandler()
{
	//Nothing
}

void TachHandler::begin()
{
	enable(1); //Turn on the power to the sensor
	
	//Configure MCPWM Capture
	mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM_CAP_0, TACH_PULSE);
	mcpwm_capture_enable(MCPWM_UNIT_0,MCPWM_SELECT_CAP0, MCPWM_POS_EDGE, 0);
	mcpwm_isr_register(MCPWM_UNIT_0, cap_event_handler, NULL, 0, NULL);
}

int TachHandler::getRPM()
{
	//Serial.println(TachHandler_counts); //Raw counts
	//Serial.println(1000000/((TachHandler_counts*125)/10000)); //Frequency (Hz)
	//Serial.println((1000000/((TachHandler_counts*125)/10000))*60); //RPM
	
	for (int k = 0; k < countArraySize-1; k++)
	{
		countArray[k] = countArray[k+1];
	}
	
	countArray[countArraySize-1] = TachHandler_counts;
	
	memcpy(countSorted,countArray, sizeof(countArray[0])*countArraySize);
	
	int i, j;
	long a;
	
	for (i = 0; i < countArraySize; ++i) 
	{
		for (j = i + 1; j < countArraySize; ++j)
		{
			if (countSorted[i] > countSorted[j]) 
			{
				a =  countSorted[i];
				countSorted[i] = countSorted[j];
				countSorted[j] = a;
			}
		}
	}
	
	/*Serial.println("---");
	for (int k = 0; k < countArraySize; k++)
	{
		Serial.println(countArray[k]);
	}
	Serial.println("---");
	
	Serial.println("+++");
	for (int k = 0; k < countArraySize; k++)
	{
		Serial.println(countSorted[k]);
	}
	Serial.println("+++");*/
	
	double countsTemp = (double)countSorted[(countArraySize-1)/2];
	
	int rpm = (int)round(((1000000/((countsTemp*125)/10000))*60)); //Calculate RPM	
	
	if(rpm > TACH_RPM_MAX) 
		return -1;
	
	if(rpm < TACH_RPM_MIN) 
		return 0;
	
	TachHandler_Cycles_Since_Last_Pulse++;
	
	if (TachHandler_Cycles_Since_Last_Pulse > countsSinceLastPulseLimit) //Check if the MCPWM capture interrupt hasn't been called in a while (probably no input)
	{
		rpm = 0;
	}
	
	return rpm; //Return RPM
	
}

void TachHandler::enable(int en) //Turn the sensor on or off
{
	if (en == 1)
	{
		digitalWrite(TACH_EN, HIGH);
	}
	else if (en == 0)
	{
		digitalWrite(TACH_EN, LOW);
	}
}

void TachHandler::sleep()
{
	
}

#endif