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
	
private:
	void GPSWrite(char*);
	int calculateChecksum(char*);
	
	
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
	delay(1000); //Not sure why this is needed...first command is ignored if sent too soon
}

void GPSHandler::reset()
{
	GPSWrite("$PCAS10,3"); //Cold boot and load defaults
	delay(500);
}

void GPSHandler::configure()
{
	//Set Update Rate (1Hz)
	GPSWrite("$PCAS02,1000"); 
	
	
	//Select Constellations
	int GPS = 	1; 	//GPS
	int BDS = 	2; 	//BeiDou
	int GNSS = 	4;	//GLONASS
	
	int constellationTotal = GPS + BDS + GNSS; //Add or remove from calculation as required
	
	char constellationSelect[10];
	snprintf(constellationSelect, sizeof constellationSelect, "$PCAS04,%d", constellationTotal);
	GPSWrite(constellationSelect); 
	
	
	//Select NMEA sentences
	//1=on, 0=off
	int GGA = 1; //Global Positioning System Fix Data
	int GLL = 0; //Geographic Position - Latitude/Longitude
	int GSA = 1; //GNSS DOP and Active Satellites
	int GSV = 1; //GNSS Satellites in View
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

uint8_t GPSRxBuffer[128]; //UART buffer must be declared globally (belongs to tick())

void GPSHandler::tick(int passthrough) //Must be called often - updates the GPS data from the UART
{
	int GPSRxBufferLength = 0;
	
	if(Serial1.available() > 0) //If there's something in the GPS RX buffer
	{
		GPSRxBufferLength = Serial1.available(); //Get the length of the RX buffer
		
		for (int n = 0; n < GPSRxBufferLength; n++) //For each byte in the RX buffer
		{
			GPSRxBuffer[n] = Serial1.read(); //Read the byte and add it to the new buffer
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


void GPSHandler::GPSWrite(char *str)
{
	Serial1.print(str); //Send command
	Serial1.print("*");
	Serial1.printf("%02X", calculateChecksum(str)); //Send checksum
	Serial1.println(); //Send CRLF
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

#endif
