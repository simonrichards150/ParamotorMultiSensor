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
	void update();
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

void DisplayHandler::update()
{
	
}

void DisplayHandler::splash()
{
	tft.setSwapBytes(true);
	tft.pushImage(0, 0, SPLASHIMG_W, SPLASHIMG_H, splashimg);
}

void DisplayHandler::loadMainView()
{
	
}



#endif
