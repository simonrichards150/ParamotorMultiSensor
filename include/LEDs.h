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
	void toggleEnable();
	int getLEDState(); //Returns current LED state (excluding off state)
	void setAmberThreshold(double);
	void setRedThreshold(double);
	double getAmberThreshold();
	double getRedThreshold();
	void sleep();
	
private:
	int LEDState = 0;
    bool LEDEnabled = true;
	double amberThreshold = 28.5; //Threshold temperature to turn to amber
	double redThreshold = 29; //Threshold temperature to turn to red
	double hysteresisValue = 0.2; //Hysteresis for state change
	
	void setLEDColour(int); // Function to set LED colour (-1=off, 0=red, 1=amber, 2=green)
};

LEDHandler::LEDHandler()
{
	//Nothing
}

void LEDHandler::begin()
{
	if (LEDEnabled == true)
	{
		setLEDColour(0); //Green
		delay(250);
		setLEDColour(1); //Amber
		delay(500);
		setLEDColour(2); //Red
		delay(500);
		setLEDColour(1); //Amber
		delay(500);
		setLEDColour(0); //Green
	}
	else
	{
		setLEDColour(-1); //Off
	}
}

void LEDHandler::enable()
{
	LEDEnabled = true;
}

void LEDHandler::disable()
{
	LEDEnabled = false;
}

void LEDHandler::toggleEnable()
{
	LEDEnabled = !LEDEnabled;
}

void LEDHandler::tick(double temp) 
{
	if (LEDEnabled == true)
	{		

		setLEDColour(LEDState);
		
		if (temp <= -100) //Temperature error
		{
			setLEDColour(2); //Red
			return;
		}
		
		int LEDStateValue = LEDState;
		temp = round(temp*100)/100; //Round to 2 decimal places
		
		if (LEDStateValue == 0) //If currently green
		{
			if (temp >= amberThreshold)
			{
				setLEDColour(1); //Amber
			}
		}
		else if (LEDStateValue == 1) //If currently amber
		{
			if (temp >= redThreshold)
			{
				setLEDColour(2); //Red
			}
			else if (temp <= amberThreshold - hysteresisValue)
			{
				setLEDColour(0); //Green
			}
		}
		else if (LEDStateValue == 2) //If currently red
		{
			if (temp <= redThreshold - hysteresisValue)
			{
				setLEDColour(1); //Amber
			}
		}
	}
	else
	{
		setLEDColour(-1); //Off
	}
}

int LEDHandler::getLEDState()
{
	return LEDState;
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
	
	if (colour > -1)
	{
		LEDState = colour;
	}
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

void LEDHandler::sleep()
{
	disable(); //LEDs off
}


#endif