//Display Handler
//Simon Richards-Martin

#ifndef DISPLAY_H
#define DISPLAY_H
#include <Arduino.h>

#define USE_DMA_TO_TFT
#define COLOR_DEPTH 16 //Required for DMA

#include <TFT_eSPI.h>

class DisplayHandler : public TFT_eSPI //Begin class definition
{
public:

	DisplayHandler(TFT_eSPI *tft);
	void begin();
	void update();
	
	
private:
	TFT_eSPI *_tft;
	
}; //End class definition


//Class methods
DisplayHandler::DisplayHandler(TFT_eSPI *tft) //Constructor
{
	//Can't really do anything here
	_tft = tft;
}

void DisplayHandler::begin()
{
	
	_tft->init();
	_tft->setRotation(3);
	_tft->setTextColor(TFT_WHITE, TFT_BLACK, true);
	
}

void DisplayHandler::update()
{
	
}



#endif
