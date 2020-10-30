
# Discover32 Sensors WiFi example sketch

This example sketch initializes the various sensors on the Discover32 board, connects to a specified WiFi ssid, prints the IP address to the serial monitor and to the display and serves a simple web-page or a JSON with a selection of the sensors values when requested.

## Setup
<p>Replace the ssid and password to those of your local WiFi network.</p>
<p>Change the GMT time offset to that of your local time zone.</p>
<p>Change the sea level pressure to a current value near your location.</p>

## Usage
<p>Once it is connected over WiFi, the Discover32's IP address appears on the display.</p>
<p>On a device connected to the same network, connect to that IP address using a web browser to get a web page with the sensors values</p>
<p>Add "/api" at the end to get the values in JSON format</p>

## Dependencies
* [Adafruit_BMP280](https://github.com/adafruit/Adafruit_BMP280_Library)
* [Adafruit_LSM6DS](https://github.com/adafruit/Adafruit_LSM6DS)
* [PCF85063TP](https://github.com/Seeed-Studio/Grove_High_Precision_RTC_PCF85063TP)
* [Adafruit_Si7021](https://github.com/adafruit/Adafruit_Si7021)
* [Adafruit_GFX](https://github.com/adafruit/Adafruit-GFX-Library)
* [Adafruit_SSD1306](https://github.com/adafruit/Adafruit_SSD1306)
