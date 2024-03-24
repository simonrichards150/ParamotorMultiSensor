//Display Handler
//Simon Richards-Martin

#ifndef DISPLAY_H
#define DISPLAY_H
#include <Arduino.h>

#define USE_DMA_TO_TFT
#define COLOR_DEPTH 16 //Required for DMA


#include <TFT_eSPI.h>
#include "splashimg.h"

#define DISPLAY_HEIGHT 170
#define DISPLAY_WIDTH 320

#define STATUSBAR_HEIGHT 24
#define STATUSBAR_OFFSET DISPLAY_HEIGHT-STATUSBAR_HEIGHT 
#define STATUSBAR_BG 0x0801 //0x1004 //0x0841

class DisplayHandler //Begin class definition
{
public:

	DisplayHandler();
	void begin();
	void update(double, int, double, String, int, bool, String);
	void splash();
	void loadMainView();
	void setBacklight(int);
	
	
private:
	TFT_eSPI tft;
	TFT_eSprite img = TFT_eSprite(&tft);
	
	void drawBatteryIndicator(int);
	
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
	drawBatteryIndicator(batt);
	
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
	tft.fillScreen(TFT_BLACK);
	tft.setCursor(0, 0);
	
	tft.fillRoundRect(0,STATUSBAR_OFFSET,320,STATUSBAR_HEIGHT,0,STATUSBAR_BG);
	
	drawBatteryIndicator(3100);
	
	
	
}

void DisplayHandler::drawBatteryIndicator(int batt)
{
	int pc = 0;
	
	if (batt >= 4200) //Full battery
	{
		pc = 100;
	}
	else if (batt <= 3100) //Empty battery
	{
		pc = 0;
	}
	else
	{
		pc = ((batt-3100)/1100.0)*100; //Calculate battery percentage
	}
	
	//Draw battery indicator outline (white)
	img.setColorDepth(16);
	img.createSprite(32,STATUSBAR_HEIGHT-8);
	img.fillSprite(TFT_TRANSPARENT);
	img.drawRoundRect(0,0,28,STATUSBAR_HEIGHT-8,1,TFT_WHITE);
	img.fillRoundRect(27,((STATUSBAR_HEIGHT-8)/2)-3,3,6,1,TFT_WHITE);
	
	//Fill the variable part of the indicator with background colour
	img.fillRoundRect(2,2,24,STATUSBAR_HEIGHT-12,0,TFT_BLACK);
	
	//Draw battery percentage indicator (colour according to percentage)
	if (pc >= 60) //Above 60% in green
	{
		img.fillRoundRect(2,2,(24.0/100.0)*pc,STATUSBAR_HEIGHT-12,0,TFT_GREEN);
	}
	else if (pc >= 20) //Above 20% in orange
	{
		img.fillRoundRect(2,2,(24.0/100.0)*pc,STATUSBAR_HEIGHT-12,0,TFT_ORANGE);
	}
	else if (pc < 20) //<20% in red
	{
		if (pc < 5)
		{
			pc = 5; //Keep a tiny bit of red even when the percentage should show nothing
		}
		
		img.fillRoundRect(2,2,(24.0/100.0)*pc,STATUSBAR_HEIGHT-12,0,TFT_RED);
	}
	
	//Push to display
	img.pushSprite(286, STATUSBAR_OFFSET + ((STATUSBAR_HEIGHT-img.height())/2), TFT_TRANSPARENT);
	img.deleteSprite();
	
}

void DisplayHandler::setBacklight(int pc)
{
	if ((pc <= 100) && (pc >= 0))
	{
		analogWrite(LCD_BL, pc * 2.55);
	}
	else
	{
		return;
	}
}



#endif
