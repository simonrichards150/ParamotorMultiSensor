//Thermocouple Handler
//Eden Agyemang

#ifndef TEMPERATURE_H 
#define TEMPERATURE_H

#include <Arduino.h>
#include <TMP1075.h>
#include <Adafruit_MCP3421.h>

class TempHandler
{
public:
	TempHandler(); 
	void begin(); 
	double getCJT();
	double getCompTemp();
	double getUnCompTemp();
	void sleep();

private:
	Adafruit_MCP3421 adc; //ADC object
	TMP1075::TMP1075 tmp1075 = TMP1075::TMP1075(Wire); //TMP1075 object
	
	void enableFan(); //Fan is connected to tachometer switch so will always be on when the tach is enabled, this function is just to allow us to turn it on earlier if we want to
	double mVtoT(double);
	double TtomV(double);
	double thermocoupleConvert(double, double);

};

TempHandler::TempHandler() //Constructor
{
	//No need to do anything here
}

void TempHandler::begin() //Initialise and configure the ADC and TMP1075
{	
	adc.begin(0x69, &Wire); //Start the adc I2C using the Wire object - ADC is address 0x69
	delay(200); //Just in case
	enableFan(); //Turn on the fan
	
	//Configure ADC
	adc.setGain(GAIN_8X);
	adc.setResolution(RESOLUTION_18_BIT);
	adc.setMode(MODE_CONTINUOUS);
	
	tmp1075.begin(); // Initialise TMP1075 sensor
	tmp1075.setConversionMode(false); //Continuous conversion mode
}

void TempHandler::enableFan()
{
	digitalWrite(TACH_EN, HIGH); // Turn the fan on by setting the TACH_EN pin HIGH
}

double TempHandler::getCJT() //Returns the cold junction temperature (temperature reported by TMP1075)
{
	return tmp1075.getTemperatureCelsius();
}

double TempHandler::getUnCompTemp() //Returns the uncompensated raw temperature reading from the thermocouple
{
	int16_t adcValue = adc.readADC();
	double millivolts = (double)adcValue * 2048.0 / 131072.0 / 8.0;
	return mVtoT(millivolts);
}

double TempHandler::getCompTemp() //Returns the thermocouple temperature with cold junction compensation applied
{
	int16_t adcValue = adc.readADC();
	double millivolts = (double)adcValue * 2048.0 / 131072.0 / 8.0;
	return thermocoupleConvert(tmp1075.getTemperatureCelsius(), millivolts);
}

void TempHandler::sleep()
{
	adc.setMode(MODE_ONE_SHOT); //One-shot mode will put the ADC to sleep 
	tmp1075.setConversionMode(true); //Single-shot mode will put the sensor to sleep
}

//-------End main class functions - don't edit below this line--------

//Thermocouple conversion calculations
//Simon Richards-Martin
//http://www.mosaic-industries.com/embedded-systems/microcontroller-projects/temperature-measurement/thermocouple/calibration-table

//Valid for -100C to 800C
double TempHandler::mVtoT(double mV) //Convert mV to celsius
{
	double T0, v0, p1, p2, p3, p4, q1, q2, q3, vDiff, TDiff, result = 0;
	
	//Split by range (different coefficients)
	if ((mV < 4.096) && (mV >= -3.554)) //-100C to 100C
	{
		T0 = -8.7935962;
		v0 = -0.34489914;
		p1 = 25.678719;
		p2 = -0.49887904;
		p3 = -0.44705222;
		p4 = -0.044869203;
		q1 = 0.00023893439;
		q2 = -0.020397750;
		q3 = -0.0018424107;
	}
	else if ((mV < 16.397) && (mV >= 4.096)) //100C to 400C
	{
		T0 = 310.18976;
		v0 = 12.631386;
		p1 = 24.061949;
		p2 = 4.0158622;
		p3 = 0.26853917;
		p4 = -0.0097188544;
		q1 = 0.16995872;
		q2 = 0.011413069;
		q3 = -0.00039275155;
	}
	else if ((mV < 33.275) && (mV >= 16.397)) //400C to 800C
	{
		T0 = 605.72562;
		v0 = 25.148718;
		p1 = 23.539401;
		p2 = 0.046547228;
		p3 = 0.013444400;
		p4 = 0.00059236853;
		q1 = 0.00083445513;
		q2 = 0.00046121445;
		q3 = 0.000025488122;
	}
	else //Out of range
	{
		return -9999; //Error
	}
	
	vDiff = mV - v0;
	
	result = T0+((vDiff*(p1+vDiff*(p2+vDiff*(p3+p4*vDiff))))/(1+vDiff*(q1+vDiff*(q2+q3*vDiff))));
	
	return result;
}

//Valid for -20C to +70C
double TempHandler::TtomV(double T) //Convert celsius to mV
{
	double T0, v0, p1, p2, p3, p4, q1, q2, q3, vDiff, TDiff, result = 0;
	
	if ((T <= 70) && (T >= -20)) 
	{
		T0 = 25;
		v0 = 1.0003453;
		p1 = 0.040514854;
		p2 = -0.000038789638;
		p3 = -0.0000028608478;
		p4 = -0.00000000095367041;
		q1 = -0.0013948675;
		q2 = -0.000067976627;
	}
	else //Out of range
	{
		return -9999; //Error
	}
	
	TDiff = T - T0;
	
	result = v0+((TDiff*(p1+TDiff*(p2+TDiff*(p3+p4*TDiff))))/(1+vDiff*(q1+q2*TDiff)));
	
	return result;
	
}

double TempHandler::thermocoupleConvert(double coldT, double hotmV) //Apply compensation and return temperature
{
	double result = 0;
	
	result = mVtoT(TtomV(coldT)+hotmV);
	
	return result;
}


#endif