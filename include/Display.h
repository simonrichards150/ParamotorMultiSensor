//Display Handler
//Simon Richards-Martin

#ifndef DISPLAY_H
#define DISPLAY_H
#include <Arduino.h>

#include <TFT_eSPI.h>
#define USE_DMA_TO_TFT
#define COLOR_DEPTH 16 //Required for DMA

#include "NotoSansMonoSCB20.h"
#include "NotoSansBold15.h"
#include "NotoSansBold36.h"
#include "splashimg.h"

#define DISPLAY_HEIGHT 170
#define DISPLAY_WIDTH 320

#define BUTTON1_Y 24
#define BUTTON2_Y 70
#define BUTTON3_Y 114

#define STATUSBAR_HEIGHT 24
#define STATUSBAR_OFFSET DISPLAY_HEIGHT-STATUSBAR_HEIGHT //146
#define STATUSBAR_BG 0x0801 //0x1004 //0x0841

#define GAUGE_DIAMETER 100
#define GAUGE_PADDING 10
#define GAUGE_ARC_THICKNESS 6
#define GAUGE_YPOS 20

#define TEMPERATURE_MIN 0 //500
#define TEMPERATURE_MAX 50 //700
#define TEMPERATURE_BG TFT_BLACK
#define TEMPERATURE_FG 0xfba0 //TFT_ORANGE

#define RPM_MIN 0
#define RPM_MAX 10000
#define RPM_BG TFT_BLACK
#define RPM_FG 0xfe20 //0x901f //TFT_MAGENTA

class DisplayHandler //Begin class definition
{
public:

	DisplayHandler();
	void begin();
	void update(double, int, double, String, int, int, bool, String, bool);
	void splash();
	void loadMainView();
	void setBacklight(int);
	
	
private:
	TFT_eSPI tft;
	TFT_eSprite img = TFT_eSprite(&tft);
	
	void drawBatteryIndicator(int, bool);
	void drawTemperatureGauge(double);
	void drawRPMGauge(int);
	
	bool loggingstate = false;
	
}; //End class definition


//Class methods
DisplayHandler::DisplayHandler() //Constructor
{
	//Can't really do anything here
	
}

void DisplayHandler::begin()
{
	
	tft.init();
	tft.setAttribute(PSRAM_ENABLE, false);
	tft.setRotation(3);
	tft.setTextColor(TFT_WHITE, TFT_BLACK, true);
	img.setAttribute(PSRAM_ENABLE, false);
	img.setColorDepth(16);
	
}

void DisplayHandler::update(double temp, int rpm, double hdg, String fix, int sats, int batt, bool chrg, String status, bool logging)
{
	//Update the log button text
	if (logging != loggingstate) //Only update if there's a change
	{
		
		if (logging)
		{
			tft.setTextColor(TFT_BLACK);
			tft.drawString("Start Log >", DISPLAY_WIDTH-2, BUTTON1_Y);
			tft.setTextColor(TFT_WHITE);
			tft.drawString("Stop Log >", DISPLAY_WIDTH-2, BUTTON1_Y);
			loggingstate = true;
		}
		else
		{
			tft.setTextColor(TFT_BLACK);
			tft.drawString("Stop Log >", DISPLAY_WIDTH-2, BUTTON1_Y);
			tft.setTextColor(TFT_WHITE);
			tft.drawString("Start Log >", DISPLAY_WIDTH-2, BUTTON1_Y);
			loggingstate = false;
		}
	}
	
	//Update the gauges
	//Temperature
	drawTemperatureGauge(temp);
	
	//RPM
	drawRPMGauge(rpm);
	
	//Update the printed values
	
	//Update the GPS status
	
	//Update the battery status indicator
	drawBatteryIndicator(batt, chrg);
	
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
	//Basically just load the static elements, then call update
	tft.fillScreen(TFT_BLACK);
	tft.setCursor(0, 0);
	
	tft.fillRoundRect(0,STATUSBAR_OFFSET,DISPLAY_WIDTH,STATUSBAR_HEIGHT,0,STATUSBAR_BG);
	
	//Draw button text
	tft.setTextDatum(5); //Middle right
	//img.unloadFont();
	tft.setTextColor(TFT_WHITE);
	tft.setTextFont(2);
	tft.drawString("Power Off >", DISPLAY_WIDTH-2, BUTTON3_Y);
	tft.drawString("Backlight >", DISPLAY_WIDTH-2, BUTTON2_Y);

	tft.drawString("Start Log >", DISPLAY_WIDTH-2, BUTTON1_Y); //Won't be logging when this is drawn
	
	
	drawBatteryIndicator(3100, false);
	drawTemperatureGauge(3);
	drawRPMGauge(3);
	
	
}

void DisplayHandler::drawBatteryIndicator(int batt, bool chg)
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
	img.createSprite(32,STATUSBAR_HEIGHT-8);
	img.fillSprite(TFT_TRANSPARENT);
	if (chg)
	{
		img.drawRoundRect(0,0,28,STATUSBAR_HEIGHT-8,1,TFT_GREEN);
		img.fillRoundRect(27,((STATUSBAR_HEIGHT-8)/2)-3,3,6,1,TFT_GREEN);
	}
	else
	{
		img.drawRoundRect(0,0,28,STATUSBAR_HEIGHT-8,1,TFT_WHITE);
		img.fillRoundRect(27,((STATUSBAR_HEIGHT-8)/2)-3,3,6,1,TFT_WHITE);
	}
	
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

void DisplayHandler::drawTemperatureGauge(double tempC)
{
	//Convert to percent (actually per mille)
	int pc = 0;
	
	
	if (tempC >= TEMPERATURE_MAX) 
	{
		pc = 1000;
	}
	else if (tempC <= TEMPERATURE_MIN) 
	{
		pc = 0;
	}
	else
	{
		pc = (int)(((tempC-TEMPERATURE_MIN)/(TEMPERATURE_MAX-TEMPERATURE_MIN))*1000); 
	}
	

	
	//Prepare sprite
	img.createSprite(GAUGE_DIAMETER+(GAUGE_PADDING*2), GAUGE_DIAMETER+(GAUGE_PADDING*2)+26);
	img.fillSprite(TFT_BLACK);
	
	img.drawSmoothArc((GAUGE_DIAMETER+(GAUGE_PADDING*2))/2,(GAUGE_DIAMETER+(GAUGE_PADDING*2))/2+20,(GAUGE_DIAMETER/2)+4,(GAUGE_DIAMETER/2)-GAUGE_ARC_THICKNESS-4,40,320,TFT_WHITE,TFT_TRANSPARENT,true);
	
	img.drawSmoothArc((GAUGE_DIAMETER+(GAUGE_PADDING*2))/2,(GAUGE_DIAMETER+(GAUGE_PADDING*2))/2+20,(GAUGE_DIAMETER/2)+2,(GAUGE_DIAMETER/2)-GAUGE_ARC_THICKNESS-2,40,320,TFT_BLACK,TFT_WHITE,true);
	
	int angle = ((280.0/1000.0)*pc)+40;
	
	if (angle >= 41) //Don't display any arc if the percentage is too small
	{
		img.drawSmoothArc((GAUGE_DIAMETER+(GAUGE_PADDING*2))/2,(GAUGE_DIAMETER+(GAUGE_PADDING*2))/2+20,GAUGE_DIAMETER/2,(GAUGE_DIAMETER/2)-GAUGE_ARC_THICKNESS,40,angle,TEMPERATURE_FG,TFT_BLACK,true);
	}
	
	//Prepare the midpoint marker
	int xval = (GAUGE_DIAMETER+(GAUGE_PADDING*2))/2;
	img.drawWideLine(xval,22,xval,26,2,TFT_WHITE);
	img.setTextDatum(7);
	img.unloadFont();
	img.setTextFont(2);
	img.drawNumber(((TEMPERATURE_MAX-TEMPERATURE_MIN)/2)+TEMPERATURE_MIN, (GAUGE_DIAMETER+(GAUGE_PADDING*2))/2, 20);
	
	//Prepare the min/max markers
	//min
	img.drawWideLine(xval-30,122,xval-30,126,2,TFT_WHITE);
	img.setTextDatum(1);
	img.unloadFont();
	img.setTextFont(2);
	img.drawNumber(TEMPERATURE_MIN, xval-30, 128);
	//max
	img.drawWideLine(xval+30,122,xval+30,126,2,TFT_WHITE);
	img.setTextDatum(1);
	img.unloadFont();
	img.setTextFont(2);
	img.drawNumber(TEMPERATURE_MAX, xval+30, 128);
	
	//Prepare the numerical readout
	img.setTextDatum(4);
	//img.setFreeFont(&FreeSansBold9pt7b);
	img.loadFont(NotoSansMonoSCB20);
	//img.drawNumber((int)tempC, (GAUGE_DIAMETER+(GAUGE_PADDING*2))/2, (GAUGE_DIAMETER+(GAUGE_PADDING*2))/2+15);
	img.drawFloat((float)tempC, 1, (GAUGE_DIAMETER+(GAUGE_PADDING*2))/2, (GAUGE_DIAMETER+(GAUGE_PADDING*2))/2+15);
	img.drawString("C", (GAUGE_DIAMETER+(GAUGE_PADDING*2))/2+5, (GAUGE_DIAMETER+(GAUGE_PADDING*2))/2+35);
	img.unloadFont();
	img.loadFont(NotoSansBold15);
	img.drawString("o", (GAUGE_DIAMETER+(GAUGE_PADDING*2))/2-5, (GAUGE_DIAMETER+(GAUGE_PADDING*2))/2+30); //Degree Symbol made from superscript o
	
	
	//Push to display
	img.pushSprite(0,GAUGE_YPOS-20,TFT_TRANSPARENT);
	img.deleteSprite();
	
}

void DisplayHandler::drawRPMGauge(int rawRPM)
{	
	//Convert to percent (actually per mille)
	int pc = 0;
	
	if (rawRPM >= RPM_MAX) 
	{
		pc = 1000;
	}
	else if (rawRPM <= RPM_MIN) 
	{
		pc = 0;
	}
	else
	{
		pc = (int)((((double)rawRPM-RPM_MIN)/(RPM_MAX-RPM_MIN))*1000); 
	}
	
	//Prepare sprite
	img.createSprite(GAUGE_DIAMETER+(GAUGE_PADDING*2), GAUGE_DIAMETER+(GAUGE_PADDING*2)+26);
	img.fillSprite(TFT_BLACK);
	
	img.drawSmoothArc((GAUGE_DIAMETER+(GAUGE_PADDING*2))/2,(GAUGE_DIAMETER+(GAUGE_PADDING*2))/2+20,(GAUGE_DIAMETER/2)+4,(GAUGE_DIAMETER/2)-GAUGE_ARC_THICKNESS-4,40,320,TFT_WHITE,TFT_TRANSPARENT,true);
	
	img.drawSmoothArc((GAUGE_DIAMETER+(GAUGE_PADDING*2))/2,(GAUGE_DIAMETER+(GAUGE_PADDING*2))/2+20,(GAUGE_DIAMETER/2)+2,(GAUGE_DIAMETER/2)-GAUGE_ARC_THICKNESS-2,40,320,TFT_BLACK,TFT_WHITE,true);
	
	int angle = ((280.0/1000.0)*pc)+40;
	
	if (angle >= 41) //Don't display any arc if the percentage is too small
	{
		img.drawSmoothArc((GAUGE_DIAMETER+(GAUGE_PADDING*2))/2,(GAUGE_DIAMETER+(GAUGE_PADDING*2))/2+20,GAUGE_DIAMETER/2,(GAUGE_DIAMETER/2)-GAUGE_ARC_THICKNESS,40,angle,RPM_FG,TFT_BLACK,true);
	}
	
	//Prepare the midpoint marker
	int xval = (GAUGE_DIAMETER+(GAUGE_PADDING*2))/2;
	img.drawWideLine(xval,22,xval,26,2,TFT_WHITE);
	img.setTextDatum(7);
	img.unloadFont();
	img.setTextFont(2);
	img.drawNumber(((RPM_MAX-RPM_MIN)/2)+RPM_MIN, (GAUGE_DIAMETER+(GAUGE_PADDING*2))/2, 20);
	
	//Prepare the min/max markers
	//min
	img.drawWideLine(xval-30,122,xval-30,126,2,TFT_WHITE);
	img.setTextDatum(1);
	img.unloadFont();
	img.setTextFont(2);
	img.drawNumber(RPM_MIN, xval-30, 128);
	//max
	img.drawWideLine(xval+30,122,xval+30,126,2,TFT_WHITE);
	img.setTextDatum(1);
	img.unloadFont();
	img.setTextFont(2);
	img.drawNumber(RPM_MAX, xval+30, 128);
	
	//Prepare the numerical readout
	img.setTextDatum(4);
	//img.setFreeFont(&FreeSansBold9pt7b);
	img.loadFont(NotoSansMonoSCB20);
	img.drawNumber(rawRPM, (GAUGE_DIAMETER+(GAUGE_PADDING*2))/2, (GAUGE_DIAMETER+(GAUGE_PADDING*2))/2+15);
	img.drawString("RPM", (GAUGE_DIAMETER+(GAUGE_PADDING*2))/2, (GAUGE_DIAMETER+(GAUGE_PADDING*2))/2+35);
	
	//Push to display
	img.pushSprite((GAUGE_DIAMETER+(GAUGE_PADDING*2)),GAUGE_YPOS-20,TFT_TRANSPARENT);
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
