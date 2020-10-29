
# Discover32 Sensors OLED example sketch

This example sketch initializes the various sensors on the Discover32 board, connects to a WiFi network to set time from a ntp server and cyclically displays to the screen a scrolling bitmap logo and a selection of the values provided by the sensors.

## Setup
Replace the ssid and password to those of your local WiFi network.
Change the GMT time offset to that of your local time zone.
Change the sea level pressure to a current value near your location.


## Dependencies
* [Adafruit_BMP280](https://github.com/adafruit/Adafruit_BMP280_Library)
* [Adafruit_LSM6DS](https://github.com/adafruit/Adafruit_LSM6DS)
* [PCF85063TP](https://github.com/Seeed-Studio/Grove_High_Precision_RTC_PCF85063TP)
* [Adafruit_Si7021](https://github.com/adafruit/Adafruit_Si7021)
* [Adafruit_GFX](https://github.com/adafruit/Adafruit-GFX-Library)
* [Adafruit_SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
