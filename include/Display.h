//Display Handler
//Simon Richards-Martin

#ifndef DISPLAY_H
#define DISPLAY_H
#include <Arduino.h>

#define USE_DMA_TO_TFT
#define COLOR_DEPTH 16 //Required for DMA


#include <TFT_eSPI.h>
#include "splashimg.h"

class DisplayHandler //Begin class definition
{
public:

	DisplayHandler();
	void begin();
	void update(double, int, double, String, int, bool, String);
	void splash();
	void loadMainView();
	
	
private:
	TFT_eSPI tft;
	
}; //End class definition


//Class methods
DisplayHandler::DisplayHandler() //Constructor
{
	//Can't really do anything here
	
}

void DisplayHandler::begin()
{
	
	tft.init();
	tft.setRotation(3);
	tft.setTextColor(TFT_WHITE, TFT_BLACK, true);
	
	//tft.fillScreen(TFT_ORANGE);
  
	
}

void DisplayHandler::update(double temp, int rpm, double hdg, String fix, int batt, bool chrg, String status)
{
	//Update the gauges
	
	//Update the printed values
	
	//Update the GPS status
	
	//Update the battery status indicator
	
	//Update the heading indicator
	
	//Update the status bar
}

void DisplayHandler::splash()
{
	tft.setSwapBytes(true);
	tft.pushImage(0, 0, SPLASHIMG_W, SPLASHIMG_H, splashimg); //Write the splash image to the display buffer
}

void DisplayHandler::loadMainView()
{
	//Basically just load the background, then call update
}



#endif
