
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

void setup() {
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.begin(115200);

  Serial.println("Discover32 Sensors WiFi test");

  if (!bmp280.begin(BMP_I2C_ADDR, BMP280_CHIPID)) {
    Serial.println("Could not find a valid BMP280 sensor!");
    while (1);
  }
  if (!lsm6ds3.begin_I2C(LSM_I2C_ADDR, &Wire, LSM6DS3_CHIP_ID)) {
    Serial.println("Could not find a valid LSM6DS3 sensor!");
    while (1);
  }
  if (!si7021.begin()) {
    Serial.println("Could not find a valid Si7021 sensor!");
    while (1);
  }
  if(!display.begin(SSD1306_SWITCHCAPVCC, SSD_I2C_ADDR)) {
    Serial.println("Could not find a valid SSD1306 screen!");
    while (1);
  }
  RTclock.begin();

  pinMode(17, OUTPUT);
  digitalWrite(17, !LOW);                                       // turn off led (logic inverted)

  display.clearDisplay();                                       // clear the display buffer
  display.setTextColor(SSD1306_WHITE);                          // set draw color to white
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.println(ssid);

    display.clearDisplay();                                     // clear the display buffer
    display.setTextSize(1);                                     // set text size to small (1:1 scale)
    display.setCursor(0,20);                                    // start at position (X,Y), starting from top left
    display.println("Connecting to");                           // print line to display buffer
    display.println(ssid);
    display.display();                                          // update display to show what is on the buffer
    
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      led_enabled = !led_enabled;
      digitalWrite(17, led_enabled);
      delay(500);
    }

    digitalWrite(17, !LOW);                                     // turn off led (logic inverted)
    
    Serial.println("WiFi connected");

    display.println("WiFi connected");
    display.display();
    
    configTime(gmtOffset * 3600, 0, ntpServer);
    updateRTclock();

    server.on("/", handleRoot);
    server.on("/api", handleAPI);
    server.onNotFound([]() {
      server.send(404, "text/plain", "Page not found");
    });
    server.begin();
    
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0,20);
    display.println(WiFi.localIP());
    display.display();
    
    digitalWrite(17, !HIGH);                                    // turn on led (logic inverted)
  }
  server.handleClient();
}

void updateRTclock() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }

  if (timeinfo.tm_isdst > 0) {
    configTime(gmtOffset * 3600, 3600, ntpServer);
    getLocalTime(&timeinfo);
  }
  
  RTclock.stopClock();
  RTclock.fillDayOfWeek(timeinfo.tm_wday);
  RTclock.fillByYMD(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);
  RTclock.fillByHMS(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  RTclock.setTime();
  RTclock.startClock();
}

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
