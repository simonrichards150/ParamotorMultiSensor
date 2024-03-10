//GPS Handler
//Simon Richards-Martin

#ifndef GPS_H
#define GPS_H
#include <Arduino.h>

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
	
	//It's not nice to expose all the variables but it's easier than writing accessor methods for all of them
	String fix, lat, lon, latNS, lonEW, alt, sats, pdop, hdop, vdop, hdg, spd, mins, secs, ms, hours, day, month, year = "";
	
	
	
private:
	char NMEAParserBuffer[128];
	char GPSRxBuffer[1024];
	int NMEAParserIndex = 0;
	
	void GPSWrite(char*);
	int calculateChecksum(char*);
	void extractNMEA(char*, int);
	
}; //End class definition


//Class methods
GPSHandler::GPSHandler() //Constructor
{
	//Can't really do anything here
}

void GPSHandler::begin() //Initialise GPS
{
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
	int GPSUpdateRate = 2; //Update rate in Hz
	
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
		digitalWrite(GPS_EN, HIGH);
	}
	else
	{
		digitalWrite(GPS_EN, LOW);
	}
}



void GPSHandler::tick(int passthrough) //Must be called often - updates the GPS data from the UART
{
	int GPSRxBufferLength = 0;
	
	if(Serial1.available() > 0) //If there's something in the GPS RX buffer
	{
		GPSRxBufferLength = Serial1.available(); //Get the length of the RX buffer
		
		for (int n = 0; n < GPSRxBufferLength; n++) //For each byte in the RX buffer
		{
			GPSRxBuffer[n] = Serial1.read(); //Read the byte and add it to the new buffer
			parseNMEA(GPSRxBuffer[n]); //Send the byte to the NMEA parser
		}
		
		if (passthrough) //If passthrough mode is selected, copy the GPS data to the PC UART
		{
			for (int n = 0; n < GPSRxBufferLength; n++) //For each byte in the stored buffer
			{
				Serial.write(GPSRxBuffer[n]); //Read the byte and add it to the PC TX buffer
			}
		}
	}
}


void GPSHandler::GPSWrite(char *str) //Send something to GPS receiver
{
	Serial1.print(str); //Send command
	Serial1.print("*");
	Serial1.printf("%02X", calculateChecksum(str)); //Send checksum
	Serial1.println(); //Send CRLF
	
	/*
	Serial.print(str); //Send command
	Serial.print("*");
	Serial.printf("%02X", calculateChecksum(str)); //Send checksum
	Serial.println(); //Send CRLF
	*/
}

int GPSHandler::calculateChecksum(char *str) //Calculate checksum for NMEA messages
{
	int crc = 0;
	int len = strlen(str);
	
	for (int i = 1; i < len; i++) 
	{
        crc ^= str[i];
    }

	return crc;
	
}



void GPSHandler::parseNMEA(char c) //Assemble NMEA sentences from received data 
{
	if(c == '$') //Start of NMEA sentence
	{
		extractNMEA(NMEAParserBuffer, NMEAParserIndex); //Deal with the previous sentence
		
		/*
		for (int n = 0; n < NMEAParserIndex; n++) 
			{
				Serial.write(NMEAParserBuffer[n]); 
			}
		*/
		memset(NMEAParserBuffer, '\0', sizeof(NMEAParserBuffer)); //Clear the buffer ready for a new sentence
		NMEAParserIndex = 0; //Reset the index
	}
	
	NMEAParserBuffer[NMEAParserIndex] = c; //Append latest byte to buffer
	NMEAParserIndex++; //Increment buffer index
}

void GPSHandler::extractNMEA(char *str, int len) //Parse each NMEA sentence and separate the fields (Checksums are ignored)
{
	//Separate the NMEA sentence into individual fields
	//Serial.printf("%s", str);
	char* token;
	String NMEAParts[21];
	int i;
	int NMEAPartsCount = 0;
	while((token = strsep(&str, ","))) 
	{
		if(*token == '\0') //If null value
		{
			NMEAParts[i] = "NULL";
		}
		else //If real value
		{
			NMEAParts[i] = String(token);
		}
		i++;
		NMEAPartsCount++;
	}
	/*
	for (int n=0; n<NMEAPartsCount; n++)
		{
			Serial.printf("%2d: %s\n", n, NMEAParts[n]);
		}
	*/		
			
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
		
		pdop = NMEAParts[15]; //Position Dilution of Precision
		hdop = NMEAParts[16]; //Horizontal Dilution of Precision
		vdop = NMEAParts[17]; //Vertical Dilution of Precision
	}
	else if (NMEAParts[0] == "$GNVTG")
	{
		hdg = NMEAParts[1]; //Course over Ground, relative to true north (Degrees)
		spd = NMEAParts[7]; //Ground speed (KPH)
	}
	else if (NMEAParts[0] == "$GNZDA")
	{
		
		//Check if there is a time provided
		if ((NMEAParts[1] == "NULL") || (NMEAParts[1] == ""))
		{
			//Invalid time
			return;
		}
		else
		{		
		hours = NMEAParts[1].substring(0, 2);
		mins = NMEAParts[1].substring(2, 4);
		secs = NMEAParts[1].substring(4, 6);
		ms = NMEAParts[1].substring(7, 10); //ms will always be 000 unless an update rate greater than 1Hz is selected
		day = NMEAParts[2];
		month = NMEAParts[3];
		year = NMEAParts[4];
		}
	}
	else //Discard anything else
	{
		//Serial.println("A malformed NMEA sentence was discarded");
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

#endif
