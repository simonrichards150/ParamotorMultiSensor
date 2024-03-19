//Pin definitions and initialisation for Paramotor Multi Sensor
//Simon Richards-Martin
//Usage: #include "include/Init.h", call pinSetup() and commsSetup() once in setup() routine

/*
	General IDE Configuration
	Board: ESP32S3 Dev Module (from Espressif esp32 board package)
	USB CDC On Boot: Enabled
	CPU Frequency: 240MHz (Wifi)
	Core Debug Level: None
	USB DFU On Boot: Disabled
	Erase All Flash Before Sketch Upload: Disabled
	Events Run On: Core 1
	Flash Mode: QIO 80Mhz
	Flash Size: 4MB (32Mb)
	JTAG Adapter: Disabled
	Arduino Runs On: Core 1
	USB Firmware MSC On Boot: Disabled
	Partition Scheme: Huge APP (3MB No OTA/1MB SPIFFS)
	PSRAM: QSPI PSRAM
	Upload Mode: UART0/Hardware CDC
	Upload Speed: 921600
	USB Mode: Hardware CDC and JTAG	
*/

#ifndef INIT_H
#define INIT_H

//Buttons - idle low, high when pressed
#define BTN1	1	//Top button
#define BTN2	2	//Middle button
#define BTN3	3	//Bottom button

/* 	
	LED usage
	The LEDs each have a source and a sink that need to be considered. 
	The sinks are controlled by MOSFETs and are common to cathodes of each LED.
	The sources are common to all LEDs individual anodes and control the colour. See schematic.
	i.e. sinks are used to select an LED, sources are used to select a colour.
	e.g. to set the first LED to red, the sink for LED1 must be turned on,
	and the source for red must be turned on:
	digitalWrite(LED1, HIGH);
	digitalWrite(LEDR, HIGH);
	Any unused LED sink/sources should be turned off.
*/

//LED sink MOSFETs - low=off, high=on
#define LED1	17	//Left LED
#define LED2	13	//Centre LED
#define LED3	12	//Right LED

//LED source pins - low=off, high=on
#define LEDR	15	//Red
#define LEDG	14	//Green
#define LEDB	16	//Blue

//Battery management
#define VBAT	4	//Battery voltage - 1:2 divider (multiply ADC reading by 1.5 to get true voltage)
#define CHG		18	//Charge status - charging=low, not charging=high

//GPS
#define GPS_EN 	43	//GPS power enable
#define GPS_TXO	40	//GPS UART TX (ESP32 RX)
#define GPS_RXI	39	//GPS UART RX (ESP32 TX)

//MicroSD
#define SD_CD	5	//Card detect - inserted=low, not inserted=high
#define SD_CLK	8
#define SD_CMD	9
#define SD_D0	7
#define SD_D1	6
#define SD_D2	11
#define SD_D3	10

//Thermocouple ADC and reference temperature sensor use I2C bus
#define TC_SDA	38
#define TC_SCL	37

//Display 
#define LCD_RES		48
#define LCD_CS		47
#define LCD_CLK		33
#define LCD_DC		34
#define LCD_MOSI	35
#define LCD_BL		36

//Tachometer
#define TACH_EN		42	//Power enable for external sensor - high=on, low=off
#define TACH_PULSE	41	//Pulse input

#include <Arduino.h>
#include <Wire.h>

void pinSetup()
{
	pinMode(BTN1, INPUT);
	pinMode(BTN2, INPUT);
	pinMode(BTN3, INPUT);
	
	pinMode(LED1, OUTPUT);
	pinMode(LED2, OUTPUT);
	pinMode(LED3, OUTPUT);
	pinMode(LEDR, OUTPUT);
	pinMode(LEDG, OUTPUT);
	pinMode(LEDB, OUTPUT);
	
	pinMode(VBAT, INPUT);
	pinMode(CHG, INPUT_PULLUP);
	
	pinMode(GPS_EN, OUTPUT);
	pinMode(GPS_TXO, INPUT);
	pinMode(GPS_RXI, OUTPUT);
	
	pinMode(SD_CD, INPUT);
	pinMode(SD_CLK, OUTPUT);
	pinMode(SD_CMD, OUTPUT);
	pinMode(SD_D0, OUTPUT);
	pinMode(SD_D1, OUTPUT);
	pinMode(SD_D2, OUTPUT);
	pinMode(SD_D3, OUTPUT);
	
	pinMode(TC_SDA, OUTPUT);
	pinMode(TC_SCL, OUTPUT);
	
	pinMode(LCD_RES, OUTPUT);
	pinMode(LCD_CS, OUTPUT);
	pinMode(LCD_CLK, OUTPUT);
	pinMode(LCD_DC, OUTPUT);
	pinMode(LCD_MOSI, OUTPUT);
	pinMode(LCD_BL, OUTPUT);
	
	pinMode(TACH_EN, OUTPUT);
	pinMode(TACH_PULSE, INPUT_PULLDOWN);	
}

void commsSetup()
{
	Serial.setTxTimeoutMs(0); //Avoid delays if USB isn't connected
	Serial.begin(115200); //Start serial to PC
	
	Wire.setPins(TC_SDA, TC_SCL); //Set up I2C pins
	Wire.begin(); //Start I2C
}

#endif