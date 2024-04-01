# Paramotor Multi Sensor

## Prerequisites

* [Espressif ESP32 Arduino Core](https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html) (V2.14 Working) (Follow instructions to install from the Arduino IDE)
* [TFT_eSPI Library](https://github.com/Bodmer/TFT_eSPI) (V2.5.43 Working) (Available in Arduino library repository - install from Arduino IDE)
  
**NOTE: TFT_eSPI Requires configuration by copying the contents of /include/TFT_eSPI_Config directory from this repository to your arduino libraries folder (probably /Documents/Arduino/libraries/TFT_eSPI) and overwriting existing files if prompted.**

* [Adafruit MCP3421 Library](https://github.com/adafruit/Adafruit_MCP3421) (V1.0.0 Working) (Available in Arduino library repository - install from Arduino IDE)
* [OneButton Library](https://github.com/mathertel/OneButton) (V2.5.0 Working) (Available in Arduino library repository - install from Arduino IDE)
* [TMP1075 Library](https://github.com/PatrickBaus/Arduino-TMP1075) (V2.1.0 Working) (Requires manual install - copy repository to your arduino libraries folder (probably /Documents/Arduino/libraries))

  ## Board Configuration Options
* Board: ESP32S3 Dev Module (from Espressif esp32 board package)
* USB CDC On Boot: Enabled
* CPU Frequency: 240MHz (Wifi)
* Core Debug Level: None
* USB DFU On Boot: Disabled
* Erase All Flash Before Sketch Upload: Disabled
* Events Run On: Core 1
* Flash Mode: QIO 80Mhz
* Flash Size: 4MB (32Mb)
* JTAG Adapter: Disabled
* Arduino Runs On: Core 1
* USB Firmware MSC On Boot: Disabled
* Partition Scheme: Huge APP (3MB No OTA/1MB SPIFFS)
* PSRAM: QSPI PSRAM
* Upload Mode: UART0/Hardware CDC
* Upload Speed: 921600
* USB Mode: Hardware CDC and JTAG	
  
