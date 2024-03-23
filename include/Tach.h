//Tachometer Handler
//Simon Richards-Martin

#ifndef TACH_H
#define TACH_H

#include <Arduino.h>
#include <soc/mcpwm_struct.h>
#include <driver/mcpwm.h>

class TachHandler
{
public:
	TachHandler();
	void begin();
	int getRPM();
	void enable(int);

private:
	//No private members
	
};

//Begin global code section (ISR can't be inside class)

double TachHandler_counts = 0;
double TachHandler_prev = 0;

portMUX_TYPE timer_mux = portMUX_INITIALIZER_UNLOCKED;

void cap_event_handler(void *arg)
{
	portENTER_CRITICAL_ISR(&timer_mux); // Disable other interrupts temporarily
	TachHandler_counts = MCPWM0.cap_chn[0].val - TachHandler_prev; // Calculate number of cycles between last pulse n and pulse n-1 (12.5ns/cycle)

	if (TachHandler_counts < 10000){ // Ignore pulses shorter than 125us (12.5ns*10000=8kHz)
		TachHandler_counts = TachHandler_prev;
	}

	TachHandler_prev = MCPWM0.cap_chn[0].val; 
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
	//Serial.println(counts); //Raw counts
	//Serial.println(1000000/((counts*125)/10000)); //Frequency (Hz)
	//Serial.println((1000000/((counts*125)/10000))*60); //RPM
	
	if(TachHandler_counts > 11500000) //Return 0 if less than 420 RPM
		return 0;
	
	return (int)round(((1000000/((TachHandler_counts*125)/10000))*60)); //Return RPM
	
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


#endif