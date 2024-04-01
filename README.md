# Paramotor Multi Sensor

## Features

- [x] K-type thermocouple exhaust temperature sensing (-100°C - +800°C, cold junction -20°C - +70°C)
- [x] RPM sensing (Input frequency 5Hz - 500Hz, configurable 0.5-2 sparks per revolution)
- [x] GPS for time, location, speed, heading
- [x] Colour IPS display for real-time sensor data
- [x] Warning light system for over-temperature events
- [x] Logging data to MicroSD card
- [x] Li-po battery and charge management (1000mAh - typical battery life 6 hours)
- [ ] Low-power sleep mode (xxx uA)
- [ ] WiFi/Bluetooth connectivity

## User-configurable options

### Tach.h

+ sparksPerRevolution sets the number of sparks per revolution of the crankshaft (default: 2)

### Display.h

+ TEMPERATURE_MAX sets the maximum value of the temperature gauge 
+ TEMPERATURE_MIN sets the minimum value of the temperature gauge  
+ RPM_MAX sets the maximum value of the RPM gauge
+ RPM_MIN sets the minimum value of the RPM gauge

(Note: gauge mid-point markers are calculated automatically)

### LEDs.h

+ amberThreshold sets the rising temperature threshold to switch from green to amber
+ redThreshold sets the rising temperature threshold to switch from amber to red

(Note: rising thresholds are exact, falling thresholds are subject to hysteresis)

## Programming

### Prerequisites

- [Download the ParamotorMultiSensor repository](https://github.com/simonrichards150/ParamotorMultiSensor/archive/refs/heads/main.zip)

**NOTE: The downloaded zip should be extracted and the repository folder should be renamed to ParamotorMultiSensor, and placed into your Arduino projects folder (probably /Documents/Arduino). i.e. You should have the folder /Documents/Arduino/ParamotorMultiSensor containing ParamotorMultiSensor.ino, /include, etc.** 

**If the Arduino IDE asks if you want to create a folder for the project when you try to open it, something has gone wrong!**

- [Arduino IDE 2.x](https://www.arduino.cc/en/software) (V2.3.2 Working)
- [Espressif ESP32 Arduino Core](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html) (V2.14 Working) (Follow instructions to install from the Arduino IDE)
- [TFT_eSPI Library](https://github.com/Bodmer/TFT_eSPI) (V2.5.43 Working) (Available in Arduino library repository - install from Arduino IDE)
  
**NOTE: TFT_eSPI Requires configuration by copying the contents of /include/TFT_eSPI_Config directory from this repository to your Arduino libraries TFT_eSPI folder (probably /Documents/Arduino/libraries/TFT_eSPI) and overwriting existing files if prompted.**

- [Adafruit MCP3421 Library](https://github.com/adafruit/Adafruit_MCP3421) (V1.0.0 Working) (Available in Arduino library repository - install from Arduino IDE)
- [OneButton Library](https://github.com/mathertel/OneButton) (V2.5.0 Working) (Available in Arduino library repository - install from Arduino IDE)
- [TMP1075 Library](https://github.com/PatrickBaus/Arduino-TMP1075) (V2.1.0 Working) (Requires manual install - copy repository to your Arduino libraries folder (probably /Documents/Arduino/libraries))

### Board Configuration
  
+ Board: ESP32S3 Dev Module (from Espressif esp32 board package)
+ USB CDC On Boot: Enabled
+ CPU Frequency: 240MHz (Wifi)
+ Core Debug Level: None
+ USB DFU On Boot: Disabled
+ Erase All Flash Before Sketch Upload: Disabled
+ Events Run On: Core 1
+ Flash Mode: QIO 80Mhz
+ Flash Size: 4MB (32Mb)
+ JTAG Adapter: Disabled
+ Arduino Runs On: Core 1
+ USB Firmware MSC On Boot: Disabled
+ Partition Scheme: Huge APP (3MB No OTA/1MB SPIFFS)
+ PSRAM: QSPI PSRAM
+ Upload Mode: UART0/Hardware CDC
+ Upload Speed: 921600
+ USB Mode: Hardware CDC and JTAG	

# Credits

+ Simon Richards-Martin (Hardware, Firmware)
+ Mozes Baiden (Firmware)
+ Eden Agyemang (Firmware)
+ Joseph Odukoya (Mechanical)
