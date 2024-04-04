//MicroSD Handler
//Mozes Baiden

#ifndef MICROSD_H
#define MICROSD_H

#include <Arduino.h>
#include <FS.h>
#include <SD_MMC.h>
#include <Ticker.h>
#include <time.h>

class MicroSDHandler
{
public:
	MicroSDHandler();
	void begin();
	void setInterval(int);
	void startLogging();
	void stopLogging();
	void toggleLogging();
	void appendLog(); //This will be called by the ticker to actually write to the MicroSD
	void tick(int, String, String, String, String, String, int, double, int, int, double, double, double, int, double, int, int); //This is called as often as possible to load new values ready to be written
	int getStatus(); //Status 0=idle, 1=logging, -1=general error, 2=no card inserted, 3=low battery
	void sleep();

private:

	Ticker logTicker;
	fs::File logFile;

	int logStatus = 0;
	int logLineNum = 0;
	int logInterval = 1;
	String logFileName = "";
	
	int logTime, logHdg, logAlt, logSats, logRPM, logVbat, logLEDState = -1;
	String logFix, logLat, logNS, logLon, logEW = "NULL";
	double logSpeed, logHdop, logVdop, logPdop, logTemp = 0;
	
	static void ticker_trigger(MicroSDHandler* objectPtr) //Needed to be able to use the ticker from inside the class
	{
		objectPtr->appendLog();
    }
	
	String getCurrentDate(bool);
	String getCurrentTime(bool);
	
};


MicroSDHandler::MicroSDHandler()
{
	//Nothing
}

void MicroSDHandler::begin()
{
	SD_MMC.setPins(SD_CLK, SD_CMD, SD_D0, SD_D1, SD_D2, SD_D3);	
}

void MicroSDHandler::setInterval(int secs)
{
	logInterval = secs;
}

void MicroSDHandler::toggleLogging()
{
	if (logStatus != 1)
	{
		startLogging();
	}
	else if (logStatus == 1)
	{
		stopLogging();
	}
}

void MicroSDHandler::startLogging()
{
	if (logFile) //If a file is open, don't make another one
	{
		Serial.println("Already logging!");
		return;
	}
	
	if (logStatus == 2) //If no card, don't start logging
	{
		Serial.println("No card!");
		return;
	}
	
	if (logStatus == 3) //If battery is too low, don't start logging
	{
		Serial.println("Battery too low!");
		return;
	}
	
	if (!SD_MMC.begin("/microsd", false, false, SDMMC_FREQ_52M, 1)) //Initialise MicroSD
	{
		logStatus = -1;
		return;
    }
	

	//Open a new log file and start a ticker
	logLineNum = 1;
	
	
	//Come up with a unique filename, based on current time if available
	if (logTime < 1) //Time is not valid
	{
		String filenamePrefix = "LOG_";
		int filenameSuffix = 0;
		
		String filenameFull = "/" + filenamePrefix + String(filenameSuffix) + ".csv";
		
		while (SD_MMC.exists(filenameFull)) //Increment number if the file already exists
		{
			filenameSuffix++;
			filenameFull = "/" + filenamePrefix + String(filenameSuffix) + ".csv";
		}
		
		logFileName = filenameFull;
	}
	else //Time is valid
	{
		logFileName = "/LOG_" + getCurrentDate(true) + "_" + getCurrentTime(true) + ".csv"; //Don't check for existing file, not likely to conflict (one second per year)
		
		//Set system time (for file creation timestamp)
		timeval epoch = {logTime, 0};
		const timeval *tv = &epoch;
		timezone gmt = {0,0};
		const timezone *tz = &gmt;
		settimeofday(tv, tz);		
	}
	
	Serial.println(logFileName);
	
	
	
	logFile = SD_MMC.open(logFileName, FILE_APPEND); //Open the file in append mode
    if (!logFile) {
        logStatus = -1;  
		return;
    }
	
	String DL = ",";
	String logLine = "LineNum" + DL + "Date" + DL + "Time" + DL + "Fix" + DL + "Latitude" + DL + "Longitude" + DL + "Heading" + DL + "Speed" + DL + "Altitude" + DL + "NumSats" + DL + "HDOP" + DL + "VDOP" + DL + "PDOP" + DL + "RPM" + DL + "Temperature" + DL + "Vbat" + DL + "LEDState" + "\n";
	if (!logFile.print(logLine))
	{
		logStatus = -1;
		stopLogging();
	}
	
	logTicker.attach(1, ticker_trigger, this);
	logStatus = 1; //Logging
}

void MicroSDHandler::stopLogging()
{
	//Close the log file and stop ticker
	logTicker.detach();
	
	
	
	if (logFile) {  // Check if the log file is open
        logFile.close();  // Close the log file
	}
	SD_MMC.end();
	
	
	
	if (logStatus == 1)
	{
		logStatus = 0;
		Serial.println(logLineNum-1);
	}
	
}

void MicroSDHandler::appendLog()
{
	//Write a log line to the SD card from local variables
	//Check if logStatus = 1 in case there's a race condition
	
	if (logStatus == 1)
	{
		String DL = ","; //Delimiter
		
		//Negate the lat/lon if necessary
		String finalLat = "";
		String finalLon = "";
		
		if (logNS == "S")
		{
			finalLat = "-" + String(logLat);
		}
		else
		{
			finalLat = logLat;
		}
		
		if (logEW == "W")
		{
			finalLon = "-" + String(logLon);
		}
		else
		{
			finalLon = logLon;
		}
		
		//Convert LED state to human readable text
		String finalLEDState = "";
		
		if (logLEDState == -1)
		{
			finalLEDState = "OFF";
		}
		else if (logLEDState == 0)
		{
			finalLEDState = "GREEN";
		}
		else if (logLEDState == 1)
		{
			finalLEDState = "AMBER";
		}
		else if (logLEDState == 2)
		{
			finalLEDState = "RED";
		}
		
		 
		String logLine = String(logLineNum) + DL + getCurrentDate(false) + DL + getCurrentTime(false) + DL + logFix + DL + finalLat + DL + finalLon + DL + String(logHdg) + DL + String(logSpeed) + DL + String(logAlt) + DL + String(logSats) + DL + String(logHdop) + DL + String(logVdop) + DL + String(logPdop) + DL + String(logRPM) + DL + String(logTemp) + DL + String(logVbat) + DL + finalLEDState + "\n";
		if (!logFile.print(logLine))
		{
			logStatus = -1;
			stopLogging();
		}
	}
	
	logLineNum++;
	//Serial.println("Logged a line!");
}


void MicroSDHandler::tick(int time, String fix, String lat, String NS, String lon, String EW, int hdg, double speed, int alt, int sats, double hdop, double vdop, double pdop, int rpm, double temp, int vbat, int LEDState)
{
	//Update local variables
	logTime = time;
	logHdg = hdg;
	logAlt = alt;
	logSats = sats;
	logRPM = rpm;
	logVbat = vbat;
	logFix = fix;
	logLat = lat;
	logNS = NS;
	logLon = lon;
	logEW = EW;
	logSpeed = speed;
	logHdop = hdop;
	logVdop = vdop;
	logPdop = pdop;
	logTemp = temp;
	logLEDState = LEDState;

	
	if (vbat < 3100) //Check vbat and stop logging if too low
	{
		logStatus = 3;
		stopLogging();
		return;
	}
	else if ((digitalRead(SD_CD) == HIGH) && (vbat >= 3100)) //Check if SD is inserted
	{
		logStatus = 2;
		if (logFile) //Check if the card was removed during logging
		{
			stopLogging();
		}
		return;
	}
	else if ((logStatus == 2) || (logStatus == 3))
	{
		logStatus = 0;
		return;
	}
	
	
}

int MicroSDHandler::getStatus()
{
	return logStatus;
}

String MicroSDHandler::getCurrentDate(bool safeForFilename = false)
{
	if (logTime > 0)
	{
		time_t now = logTime;
		struct tm *timeInfo = gmtime(&now);
		char buffer[11];
		if (safeForFilename)
		{
			strftime(buffer, sizeof(buffer), "%Y%m%d", timeInfo);
		}
		else
		{
			strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeInfo);
		}
		
		return String(buffer);
	}
	else
	{
		return String("NULL");
	}
}

String MicroSDHandler::getCurrentTime(bool safeForFilename = false)
{
	if (logTime > 0)
	{
		time_t now = logTime;
		struct tm *timeInfo = gmtime(&now);
		char buffer[20];
		if (safeForFilename)
		{
			strftime(buffer, sizeof(buffer), "%H%M%S", timeInfo);
		}
		else
		{
			strftime(buffer, sizeof(buffer), "%H:%M:%S", timeInfo);
		}
		return String(buffer);
	}
	else
	{
		return String("NULL");
	}
}

void MicroSDHandler::sleep()
{
	//Nothing
}





#endif