
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_LSM6DS3.h>
#include <PCF85063TP.h>
#include <Adafruit_Si7021.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <WebServer.h>
#include <time.h>
  
#define I2C_SDA 21                      // I2C data line
#define I2C_SCL 22                      // I2C clock line
#define BMP_I2C_ADDR 0x76               // BMP280 I2C address
#define LSM_I2C_ADDR 0x6B               // LSM6DS3 I2C address
#define SSD_I2C_ADDR 0x3C               // SSD1306 I2C address
#define SCREEN_WIDTH 128                // OLED screen width
#define SCREEN_HEIGHT 64                // OLED screen height

Adafruit_BMP280 bmp280;
Adafruit_LSM6DS3 lsm6ds3;
PCD85063TP RTclock;
Adafruit_Si7021 si7021;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
WebServer server(80);

const char* ssid     = "yourssid";        // replace with the name of the WiFi netwoork
const char* password = "yourpassword";    // replace with the WiFi network password
const char* ntpServer = "pool.ntp.org";   // ntp pool server
const long gmtOffset = 0;                 // Greenwich Mean Time (GMT) offset, in hours
float seaLevelhPa = 1025.0;               // local sea level pressure, in hPa, to adjust altitude
bool led_enabled = false;

/* Run setup once */
void setup() {
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.begin(115200);

  Serial.println("Discover32 Sensors WiFi test");

  if (!bmp280.begin(BMP_I2C_ADDR, BMP280_CHIPID)) {                 // try to initialize BMP280 sensor
    Serial.println("Could not find a valid BMP280 sensor!");
    while (1);                                                      // loop forever
  }
  if (!lsm6ds3.begin_I2C(LSM_I2C_ADDR, &Wire, LSM6DS3_CHIP_ID)) {   // try to initialize LSM6DS3 sensor
    Serial.println("Could not find a valid LSM6DS3 sensor!");
    while (1);                                                      // loop forever
  }
  if (!si7021.begin()) {                                            // try to initialize Si7021 sensor
    Serial.println("Could not find a valid Si7021 sensor!");
    while (1);                                                      // loop forever
  }
  if(!display.begin(SSD1306_SWITCHCAPVCC, SSD_I2C_ADDR)) {          // try to initialize SSD1306 display
    Serial.println("Could not find a valid SSD1306 screen!");
    while (1);                                                      // loop forever
  }
  RTclock.begin();                                                  // Initialize Real-Time clock

  pinMode(17, OUTPUT);                                              // set led pin 17 as output
  digitalWrite(17, !LOW);                                           // turn off led (logic inverted)

  display.clearDisplay();                                           // clear the display buffer
  display.setTextColor(SSD1306_WHITE);                              // set draw color to white
}

/* Loop to keep verifying WiFi connection and handling server requests */
void loop() {
  if (WiFi.status() != WL_CONNECTED) {                              // verify there is no connection
    Serial.print("Connecting to ");
    Serial.println(ssid);

    display.clearDisplay();                                         // clear the display buffer
    display.setTextSize(1);                                         // set text size to small (1:1 scale)
    display.setCursor(0,20);                                        // start at position (X,Y), starting from top left
    display.println("Connecting to");                               // print line to display buffer
    display.println(ssid);
    display.display();                                              // update display to show what is on the buffer
    
    WiFi.begin(ssid, password);                                     // connect to the specified WiFi network
    while (WiFi.status() != WL_CONNECTED) {                         // verify it hasn't connected yet
      led_enabled = !led_enabled;                                   // toggle led state
      digitalWrite(17, led_enabled);                                // turn led on or off
      delay(500);                                                   // wait 500 ms
    }

    digitalWrite(17, !LOW);                                         // turn off led (logic inverted)
    
    Serial.println("WiFi connected");

    display.println("WiFi connected");
    display.display();
    
    configTime(gmtOffset * 3600, 0, ntpServer);                     // configure time
    updateRTclock();                                                // call function to update the real-time clock

    server.on("/", handleRoot);                                     // function handleRoot runs when a request to root is received
    server.on("/api", handleAPI);                                   // function handleAPI runs when a request to /api is received
    server.onNotFound([]() {                                        // anonymous function with a simple server response
      server.send(404, "text/plain", "Page not found");             //  runs when a request is not found
    });
    server.begin();                                                 // start server
    
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0,20);
    display.println(WiFi.localIP());
    display.display();
    
    digitalWrite(17, !HIGH);                                        // turn on led (logic inverted)
  }
  server.handleClient();                                            // handle server requests
}

/* Function to update the real-time clock from ntp */
void updateRTclock() {
  struct tm timeinfo;                                               // create time structure
  
  if(!getLocalTime(&timeinfo)){                                     // try to get time info and save it to the timeinfo structure
    Serial.println("Failed to obtain time");
    return;
  }

  if (timeinfo.tm_isdst > 0) {                                      // verify if it's summer time (DST)
    configTime(gmtOffset * 3600, 3600, ntpServer);                  // configure time with summer time offset
    getLocalTime(&timeinfo);                                        // get time info and save it to the timeinfo structure
  }
  
  RTclock.stopClock();                                                                // stop the real-time clock
  RTclock.fillDayOfWeek(timeinfo.tm_wday);                                            // fill week day from timeinfo structure
  RTclock.fillByYMD(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);  // fill year, month and day from timeinfo structure
  RTclock.fillByHMS(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);              // fill hour, minute and second from timeinfo structure
  RTclock.setTime();                                                                  // set the filled time
  RTclock.startClock();                                                               // start the real-time clock
}

/* Function that receives week day info and returns the corresponding string */
char* convertDayOfWeek(int nDayOfWeek) {
  switch (nDayOfWeek) {
    case 0:
      return "Sunday";
    case 1:
      return "Monday";
    case 2:
      return "Tuesday";
    case 3:
      return "Wednesday";
    case 4:
      return "Thursday";
    case 5:
      return "Friday";
    case 6:
      return "Saturday";
  }
}

/* Server response to root, a simple web page is delivered */
void handleRoot() {
  RTclock.getTime();
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  lsm6ds3.getEvent(&accel, &gyro, &temp);

  char message[1000];

  snprintf(message, 1000, 
"<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>Discover32</title>\
  </head>\
  <body>\
    <h1>Discover32 Sensors</h1>\
    <p>%s %d/%d/%d %02d:%02d:%02d</p>\
    <p>Temperature: %.2f C</p>\
    <p>Humidity: %.1f %%</p>\
    <p>Pressure: %.1f Pa</p>\
    <p>Altitude: %.1f m</p>\
    <p>X acceleration: %.2f m/s^2</p>\
    <p>Y acceleration: %.2f m/s^2</p>\
  </body>\
</html>",
  convertDayOfWeek(RTclock.dayOfWeek), RTclock.year + 2000, RTclock.month, RTclock.dayOfMonth, RTclock.hour, RTclock.minute, RTclock.second,
    si7021.readTemperature(), si7021.readHumidity(), bmp280.readPressure(), bmp280.readAltitude(seaLevelhPa),
    accel.acceleration.x, accel.acceleration.y);

  server.send(200, "text/html", message);
}

/* Server response to /api, the values are delivered in JSON format */
void handleAPI() {
  RTclock.getTime();
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  lsm6ds3.getEvent(&accel, &gyro, &temp);

  char message[1000];

  snprintf(message, 1000, 
"{\
\"dayOfWeek\": \"%s\",\
\"year\": %d,\
\"month\": %d,\
\"day\": %d,\
\"hour\": %d,\
\"minute\": %d,\
\"second\": %d,\
\"temperature\": %.2f,\
\"humidity\": %.1f,\
\"pressure\": %.1f,\
\"altitude\": %.1f,\
\"x-accel\": %.2f,\
\"y-accel\": %.2f\
}", 
  convertDayOfWeek(RTclock.dayOfWeek), RTclock.year + 2000, RTclock.month, RTclock.dayOfMonth, RTclock.hour, RTclock.minute, RTclock.second,
    si7021.readTemperature(), si7021.readHumidity(), bmp280.readPressure(), bmp280.readAltitude(seaLevelhPa),
    accel.acceleration.x, accel.acceleration.y);

  server.send(200, "application/json", message);
}
