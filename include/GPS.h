//GPS Handler
//Simon Richards-Martin

#ifndef GPS_H
#define GPS_H
#include <Arduino.h>
#include <time.h>

class GPSHandler //Begin class definition
{
public:
	GPSHandler();
	void begin();
	void reset();
	void configure();
	void setPwr(int);
	void tick(int);
	void parseNMEA(char);
	void printVars();
	int epochTime();
	
	
	
	bool timeValid = false;
	
	//It's not nice to expose all the variables but it's easier than writing accessor methods for all of them
	String fix, lat, lon, latNS, lonEW, alt, sats, pdop, hdop, vdop, hdg, spd, mins, secs, ms, hours, day, month, year = "";
	
	
	
	
private:
	void GPSWrite(char*);
	int calculateChecksum(char*);
	bool validNMEASentence(char*);
	void extractNMEA(char*);
	
	char NMEAParserBuffer[512] = {'\0'};
	int NMEAParserIndex = 0;
	
}; //End class definition


//Class methods
GPSHandler::GPSHandler() //Constructor
{
	//Can't really do anything here
}

void GPSHandler::begin() //Initialise GPS
{
	setPwr(0);
	delay(200);
	setPwr(1); //Turn on the GPS receiver
	Serial1.setPins(GPS_TXO, GPS_RXI); //Second serial port for GPS
	Serial1.begin(9600); //Default 9600bps
	delay(500); //Not sure why this is needed...first command is ignored if sent too soon
}

void GPSHandler::reset()
{
	GPSWrite("$PCAS10,3"); //Cold boot and load defaults
	delay(500);
}

void GPSHandler::configure()
{
	//Set Update Rate
	int GPSUpdateRate = 5; //Update rate in Hz
	
	GPSUpdateRate = 1000 / GPSUpdateRate;
	char GPSUpdateRateSelect[13];
	snprintf(GPSUpdateRateSelect, sizeof GPSUpdateRateSelect, "$PCAS02,%d", GPSUpdateRate);
	GPSWrite(GPSUpdateRateSelect); 
	
	
	//Select Constellations
	int GPS = 	1; 	//GPS
	int BDS = 	2; 	//BeiDou
	int GNSS = 	4;	//GLONASS
	
	int constellationTotal = GPS + BDS + GNSS; //Include or exclude from calculation as required
	
	char constellationSelect[10];
	snprintf(constellationSelect, sizeof constellationSelect, "$PCAS04,%d", constellationTotal);
	GPSWrite(constellationSelect); 
	
	
	//Select NMEA sentences
	//1=on, 0=off
	int GGA = 1; //Global Positioning System Fix Data
	int GLL = 0; //Geographic Position - Latitude/Longitude
	int GSA = 1; //GNSS DOP and Active Satellites
	int GSV = 0; //GNSS Satellites in View
	int RMC = 0; //Recommended Minimum Specific GNSS data
	int VTG = 1; //Course Over Ground & Ground Speed
	int ZDA = 1; //Time & Date
	int ANT = 0; //Antenna Information (Sent in TXT Message)
	
	char sentenceSelect[34];
	snprintf(sentenceSelect, sizeof sentenceSelect, "$PCAS03,%d,%d,%d,%d,%d,%d,%d,%d,0,0,,,0,0", GGA, GLL, GSA, GSV, RMC, VTG, ZDA, ANT);
	GPSWrite(sentenceSelect);	
	
}

void GPSHandler::setPwr(int pwr)
{
	if(pwr)
	{
		digitalWrite(GPS_EN, HIGH); //Turn on 
	}
	else
	{
		digitalWrite(GPS_EN, LOW); //Turn off
	}
}



void GPSHandler::tick(int passthrough) //Must be called often - updates the GPS data from the UART
{
	int GPSRxAvailable = 0;
	char currentchar = '\0';
	
	GPSRxAvailable = Serial1.available(); //Get the number of bytes available in the GPS RX buffer
	//Serial.printf("Available: %d\n", GPSRxAvailable); 
	while(Serial1.available()) //Read the GPS RX buffer contents
	{
		currentchar = Serial1.read();
		if (passthrough) //If passthrough mode is selected, copy the GPS data to the PC UART
		{
			Serial.write(currentchar);
		}
		parseNMEA(currentchar); //Send byte to NMEA parser
	}
}


void GPSHandler::GPSWrite(char *str) //Send something to GPS receiver
{
	Serial1.print(str); //Send command
	Serial1.print("*");
	Serial1.printf("%02X", calculateChecksum(str)); //Send checksum
	Serial1.println(); //Send CRLF
}

int GPSHandler::calculateChecksum(char *str) //Calculate checksum for general NMEA messages
{
	int crc = 0;
	int len = strlen(str); //Length excluding trailing null characters
	
	for (int i = 1; i < len; i++) //Ignore the leading $
	{
        crc ^= str[i];
    }

	return crc;
}

bool GPSHandler::validNMEASentence(char *str) //Validate incoming NMEA sentences
{
	//Check the minimum length
	int len = strlen(str); //Length excluding trailing null characters
	
	if (len < 10)
	{
		//Serial.printf("LENGTH New: %s\n", str);
		return false; //If sentence is suspiciously short, return error
	}
	
	
	//Remove the last two bytes (CRLF)
	str[len-1] = '\0';
	str[len-2] = '\0';
	len -= 2;
	
	
	//Check the first character is $
	if (str[0] != '$')
	{
		//Serial.printf("DOLLAR New: %s\n", str);
		return false;
	}
	
	//Check the third to last character is *
	if (str[len-3] != '*')
	{
		//Serial.printf("ASTERISK New: %s\n", str);
		return false;
	}
	
	
	//Check the last two characters form a valid hex value for CRC
	char rawCRC[4] = {'\0'}; 
	rawCRC[0] = str[len-2];
	rawCRC[1] = str[len-1];
	
	//Serial.printf("CRC: %s\n", rawCRC);
	
	if (((rawCRC[0] < 48) || (rawCRC[0] > 57)) && ((rawCRC[0] < 65) || (rawCRC[0] > 70))) 
	{
		//Serial.printf("CRC Error 1\n");
		return false;
	}
	
	if (((rawCRC[1] < 48) || (rawCRC[1] > 57)) && ((rawCRC[1] < 65) || (rawCRC[1] > 70)))
	{
		//Serial.printf("CRC Error 2\n");
		return false;
	}
	
	
	//Check CRC matches
	int crc1 = 0;
	int crc2 = 0;
	
	//Calculate expected checksum
	for (int i = 1; i < len-3; i++) //Ignore the leading $ and last 3 bytes (*DD)
	{
        crc1 ^= str[i];
    }
	
	//Convert actual checksum to int
	crc2 = (int)strtol(rawCRC, NULL, 16);
	
	if (crc1 == crc2)
	{
		//Serial.printf("CRC Match!\n");
		return true;
	}

	return false; 
	 
}



void GPSHandler::parseNMEA(char c) //Assemble NMEA sentences from received data 
{	
	if (c =='$') //Start of new NMEA sentence
	{
		if (validNMEASentence(NMEAParserBuffer))
		{
			//Serial.printf("VALID Len: %d, Sentence: %s\n", NMEAParserIndex, NMEAParserBuffer); 
			extractNMEA(NMEAParserBuffer);
		}
		else
		{
			//Serial.printf("INVALID Len: %d, Sentence: %s\n", NMEAParserIndex, NMEAParserBuffer); 
		}
		memset(NMEAParserBuffer, '\0', sizeof(NMEAParserBuffer)); //Clear the buffer
		NMEAParserIndex = 0; //Reset the buffer index
	}
	
	if (NMEAParserIndex < sizeof(NMEAParserBuffer)) //Don't overrun the buffer (in case of malformed sentences)
	{
	NMEAParserBuffer[NMEAParserIndex] = c; //Append received byte to buffer
	NMEAParserIndex++; //Increment buffer index
	}
}

void GPSHandler::extractNMEA(char *str1) //Parse each NMEA sentence and separate the fields 
{
	//Separate the NMEA sentence into individual fields
	
	//Make a copy of the sentence because strsep will modify it
	
	char* str = strdupa(str1);
	
	//Serial.print("+++");
	//Serial.printf("%s", str);
	//Serial.println("---");
	//Serial.flush();
	
	char* token;
	String NMEAParts[64]; //Shouldn't be longer than 64 elements...unless something went very wrong
	int NMEAPartsCount = 0;
	
	while((token = strsep(&str, ","))) //Split the sentence 
	{
		
		if(token[0] == '\0') //If null value
		{
			if (NMEAPartsCount < 64) //Don't overrun the array
			{
				NMEAParts[NMEAPartsCount] = "NULL"; //Write string "NULL" to the array (Not actually null)
			}
			else
			{
				return; 
			}
		}
		else //If real value
		{			
			if (NMEAPartsCount < 64) //Don't overrun the array
			{
				NMEAParts[NMEAPartsCount] = String(token); //Write the value from the sentence into the array
			}
			else
			{
				return;
			}
		}
		
		NMEAPartsCount++;
	}
		
	//Determine which type of NMEA sentence and update variables
	if(NMEAParts[0] == "$GNGGA")
	{
		lat = NMEAParts[2]; 	//Latitude (Degrees and Minutes)
		latNS = NMEAParts[3];	//Latitude Direction (North or South)
		lon = NMEAParts[4];		//Longitude (Degrees and Minutes)
		lonEW = NMEAParts[5];	//Longitude Direction (East or West)
		sats = NMEAParts[7];	//No. of Satellites in Use
		alt = NMEAParts[9];		//Altitude (M above MSL)
	}
	else if (NMEAParts[0] == "$GNGSA")
	{
		if(NMEAParts[2] == "1") //Fix type
		{
			fix = "NOFIX"; //No fix
		}
		else if(NMEAParts[2] == "2")
		{
			fix = "2D"; //2D Fix
		}
		else if(NMEAParts[2] == "3")
		{
			fix = "3D"; //3D Fix
		}
		
		//NMEAParts[15] = '\0';
		
		pdop = NMEAParts[15]; //Position Dilution of Precision
		hdop = NMEAParts[16]; //Horizontal Dilution of Precision
		vdop = NMEAParts[17]; //Vertical Dilution of Precision
	}
	else if (NMEAParts[0] == "$GNVTG")
	{
		spd = NMEAParts[7]; //Ground speed (KPH)
		
		if (spd.toInt() > 1) //If stationary, heading is likely to be wrong
		{
			hdg = NMEAParts[1]; //Course over Ground, relative to true north (Degrees)
		}
		else
		{
			hdg = "INVALID"; //Warn of incorrect heading
		}
		
	}
	else if (NMEAParts[0] == "$GNZDA")
	{
		
		//Check if there is a time provided
		if ((NMEAParts[1] == "NULL") || (NMEAParts[1] == "") || (NMEAParts[2] == "NULL") || (NMEAParts[2] == "") || (NMEAParts[3] == "NULL") || (NMEAParts[3] == "") || (NMEAParts[4] == "NULL") || (NMEAParts[4] == ""))
		{
			//Invalid time
			timeValid = false;
			return;
		}
		else
		{		
		hours = NMEAParts[1].substring(0, 2);
		mins = NMEAParts[1].substring(2, 4);
		secs = NMEAParts[1].substring(4, 6);
		ms = NMEAParts[1].substring(7, 10); //Multiples of Hz update rate
		day = NMEAParts[2];
		month = NMEAParts[3];
		year = NMEAParts[4];
		
		timeValid = true;
		}
	}
	else //Discard anything else
	{
		return;
	}
	
}

void GPSHandler::printVars() //Print all the GPS data
{
	Serial.print("Fix Type: ");
	Serial.println(fix);
	
	Serial.print("Latitude: ");
	Serial.print(lat);
	Serial.print(", ");
	Serial.println(latNS);
	
	Serial.print("Longitude: ");
	Serial.print(lon);
	Serial.print(", ");
	Serial.println(lonEW);
	
	Serial.print("Altitude: ");
	Serial.print(alt);
	Serial.println(" M");
	
	Serial.print("Satellites: ");
	Serial.println(sats);
	
	Serial.print("PDOP: ");
	Serial.println(pdop);
	
	Serial.print("HDOP: ");
	Serial.println(hdop);
	
	Serial.print("VDOP: ");
	Serial.println(vdop);
	
	Serial.print("Heading: ");
	Serial.println(hdg);
	
	Serial.print("Speed: ");
	Serial.print(spd);
	Serial.println(" KPH");
	
	Serial.print("Hours: ");
	Serial.println(hours);
	
	Serial.print("Minutes: ");
	Serial.println(mins);
	
	Serial.print("Seconds: ");
	Serial.println(secs);
	
	Serial.print("Milliseconds: ");
	Serial.println(ms);
	
	Serial.print("Day: ");
	Serial.println(day);
	
	Serial.print("Month: ");
	Serial.println(month);
	
	Serial.print("Year: ");
	Serial.println(year);
}


int GPSHandler::epochTime() //Get epoch time in time_t format - not synchronised, could be slightly out of date when called
{	
	if (timeValid)
	{
		struct tm t;
		time_t t_of_day;

		t.tm_year = year.toInt()-1900;  
		t.tm_mon = month.toInt()-1;           
		t.tm_mday = day.toInt();          
		t.tm_hour = hours.toInt();
		t.tm_min = mins.toInt();
		t.tm_sec = secs.toInt();
		t.tm_isdst = -1;        
		t_of_day = mktime(&t);
		
		return (int)t_of_day;
	}
	else
	{
		return -1; //Time is not valid
	}
}

#endif
