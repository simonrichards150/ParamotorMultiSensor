//LED Indicator Handler
//Eden Agyemang

#ifndef LEDS_H
#define LEDS_H

#include <Arduino.h>

#define leftSinkPin		LED1
#define middleSinkPin	LED2
#define rightSinkPin	LED3
#define redSourcePin	LEDR
#define greenSourcePin	LEDG
#define blueSourcePin	LEDB

class LEDHandler 
{
public:
    LEDHandler(); // Constructor
	void begin();
	void tick(double);
	void enable();
	void disable();
	int getLEDStatus();
	void setAmberThreshold(double);
	void setRedThreshold(double);
	double getAmberThreshold();
	double getRedThreshold();
	
private:
	int LEDStatus = 0;
    bool LEDEnabled = true;
	double amberThreshold = 28.5; //Threshold temperature to turn to amber
	double redThreshold = 30; //Threshold temperature to turn to red
	
	void setLEDColour(int); // Function to set LED colour (-1=off, 0=red, 1=amber, 2=green)
};

LEDHandler::LEDHandler()
{
	//Nothing
}

void LEDHandler::begin()
{
	if (LEDEnabled)
	{
		setLEDColour(0); //Green
	}
	else
	{
		setLEDColour(-1); //Off
	}
}

void LEDHandler::enable()
{
	setLEDColour(0); //Green
	LEDEnabled = true;
}

void LEDHandler::disable()
{
	LEDEnabled = false;
	setLEDColour(-1); //Off
}

void LEDHandler::tick(double temp) 
{
	if (LEDEnabled)
	{
		if (temp > redThreshold)
		{
			setLEDColour(2); //Red
		}
		else if (temp > amberThreshold)
		{
			setLEDColour(1); //Amber
		}
		else
		{
			setLEDColour(0); //Green
		}
	}
}

int LEDHandler::getLEDStatus()
{
	return LEDStatus;
}

void LEDHandler::setLEDColour(int colour) 
{
    // Turn off all LEDs initially
    digitalWrite(leftSinkPin, LOW);
    digitalWrite(middleSinkPin, LOW);
    digitalWrite(rightSinkPin, LOW);

    // Turn off all LED sources initially
    digitalWrite(redSourcePin, LOW);
    digitalWrite(greenSourcePin, LOW);
    digitalWrite(blueSourcePin, LOW);

    // Set LED colours based on the given colour parameter
    if (colour == 2) { // Red
        digitalWrite(rightSinkPin, HIGH);
        digitalWrite(redSourcePin, HIGH);
    } else if (colour == 1) { // Amber
        digitalWrite(middleSinkPin, HIGH);
        digitalWrite(redSourcePin, HIGH);
        digitalWrite(greenSourcePin, HIGH);
    } else if (colour == 0) { // Green
        digitalWrite(leftSinkPin, HIGH);
        digitalWrite(greenSourcePin, HIGH);
    }
	
	LEDStatus = colour;
}

void LEDHandler::setAmberThreshold(double newThreshold)
{
	if (newThreshold < redThreshold)
	{
		amberThreshold = newThreshold;
	}
	else
	{
		Serial.print("Amber threshold can't be greater than red threshold (");
		Serial.print(redThreshold);
		Serial.println(")");
	}
}
void LEDHandler::setRedThreshold(double newThreshold)
{
	if (newThreshold > amberThreshold)
	{
		redThreshold = newThreshold;
	}
	else
	{
		Serial.print("Red threshold must be greater than amber threshold (");
		Serial.print(amberThreshold);
		Serial.println(")");
	}
}

double LEDHandler::getAmberThreshold()
{
	return amberThreshold;
}

double LEDHandler::getRedThreshold()
{
	return redThreshold;
}



#endif