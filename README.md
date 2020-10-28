
# Globaltronic Discover32

A colection of example Arduino sketches to interact with the sensors and features present on the Globaltronic Discover32 esp32 powered board.

![DISCOVER32BOARD](/assets/DISCOVER32BOARD.png)

## Board Features
- 2 cores 32-bit microprocessor
- Ultra low power co-processor
- Wi-Fi 802.11 b/g/n
- Bluetooth v4.2 and BLE
- Si7021 Humidity and Temperature Sensor
- BMP280 Digital Pressure Sensor
- LSM6DS3 iNEMO 3D Accelerometer and Gyroscope
- Si1133 UV Index and Ambient Light Sensor
- CCS811 Gas Sensor for Monitoring Indoor Air Quality
- PCF85063TP Real-Time Clock/Calendar
- ATECC508A Cryptographic Accelerator with Secure Hardware-based Key Storage
- WS2812B Addressable RGB Led

## Dependencies
* [Adafruit_BMP280](https://github.com/adafruit/Adafruit_BMP280_Library)
* [Adafruit_LSM6DS](https://github.com/adafruit/Adafruit_LSM6DS)
* [PCF85063TP](https://github.com/Seeed-Studio/Grove_High_Precision_RTC_PCF85063TP)
* [Adafruit_Si7021](https://github.com/adafruit/Adafruit_Si7021)
* [Adafruit_GFX](https://github.com/adafruit/Adafruit-GFX-Library)
* [Adafruit_SSD1306](https://github.com/adafruit/Adafruit_SSD1306)

## Installation Instructions
- Install the [Arduino IDE](https://www.arduino.cc/en/Main/Software)
- Install [arduino-esp32 using the Boards Manager](https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md)
- Download the latest [release](https://github.com/Globaltronic/discover32/releases) as zip
- [Import the .zip library](https://www.arduino.cc/en/guide/libraries#toc4)
- Install the library dependencies listed above [using the Library Manager](https://www.arduino.cc/en/guide/libraries#toc3)

## How to use
- Connect the Discover32 board to the computer with an USB cable
- Start the Arduino IDE
- Tools -> Board -> ESP32 Arduino -> ESP32 Dev Module
- Tools -> Port -> choose the available port
- File -> Examples -> Examples from Custom Libraries: Globaltronic Discover32 -> pick an example of your choice
- Click the Upload arrow to compile the sketch and flash to the board
- To monitor serial messages: Tools -> Serial Monitor. Change to 115200 baud

## Examples
- [BMP280 Digital Pressure Sensor](https://github.com/Globaltronic/discover32/tree/master/examples/BMP280_Digital_Pressure_Sensor)
- [LSM6DS3 3D Accelerometer Gyroscope](https://github.com/Globaltronic/discover32/tree/master/examples/LSM6DS3_3D_Accelerometer_Gyroscope)
- [Si7021 Humidity and Temperature Sensor](https://github.com/Globaltronic/discover32/tree/master/examples/Si7021_Humidity_and_Temperature_Sensor)
- [PCF85063TP Real-Time Clock and Calendar](https://github.com/Globaltronic/discover32/tree/master/examples/PCF85063TP_Real-Time_Clock_Calendar)
- [SSD1306 OLED Screen](https://github.com/Globaltronic/discover32/tree/master/examples/SSD1306_OLED)
- [Discover32 Sensors](https://github.com/Globaltronic/discover32/tree/master/examples/Discover32_Sensors)
- [Discover32 Sensors OLED](https://github.com/Globaltronic/discover32/tree/master/examples/Discover32_Sensors_OLED)
- [Discover32 Sensors WiFi](https://github.com/Globaltronic/discover32/tree/master/examples/Discover32_Sensors_WiFi)
