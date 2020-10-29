#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_LSM6DS3.h>
#include <PCF85063TP.h>
#include <Adafruit_Si7021.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
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

  Serial.println("Discover32 Sensors OLED test");

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
  display.setTextSize(1);                                           // set text size to small (1:1 scale)
  display.setTextColor(SSD1306_WHITE);                              // set draw color to white
  display.setCursor(0,4);                                           // start at position (X,Y), starting from top left
  display.println("Connecting to");                                 // print line to display buffer
  display.println(ssid);
  display.display();                                                // update display to show what is on the buffer

  WiFi.begin(ssid, password);                                       // connect to the specified WiFi network
  int count = 0;
  while (WiFi.status() != WL_CONNECTED) {                           // verify it hasn't connected yet
    led_enabled = !led_enabled;                                     // toggle led state
    digitalWrite(17, led_enabled);                                  // turn led on or off
    if (count == 20) {                                              // verify if the loop has been running for 10 seconds (20x500ms)
      WiFi.mode(WIFI_OFF);                                          // turn off WiFi
      digitalWrite(17, !LOW);                                       // turn off led (logic inverted)
      
      display.println("Connection Failed");
      display.display(); 
      
      setSpecificTime();                                            // set a specified time and date

      delay(2000);                                                  // wait 2 seconds
      return;
    }
    count++;                                                        // increase counter
    delay(500);                                                     // wait 500 ms
  }

  digitalWrite(17, !HIGH);                                          // turn on led (logic inverted)

  display.println("Setting ntp time");
  display.display();
    
  configTime(gmtOffset * 3600, 0, ntpServer);                       // configure time

  struct tm timeinfo;                                               // create time structure
  
  if(!getLocalTime(&timeinfo)){                                     // try to get time info and save it on the timeinfo structure
    display.println("Failed to obtain time");
    display.display();

    WiFi.disconnect(true);                                          // disconnect from the WiFi network
    WiFi.mode(WIFI_OFF);
    digitalWrite(17, !LOW);                                         // turn off led (logic inverted)
    
    setSpecificTime;                                                // set a specified time and date

    delay(2000);
    return;
  }

  if (timeinfo.tm_isdst > 0) {
    configTime(gmtOffset * 3600, 3600, ntpServer);
    getLocalTime(&timeinfo);
  }

  RTclock.stopClock();                                                                // stop the real-time clock
  RTclock.fillDayOfWeek(timeinfo.tm_wday);                                            // fill week day from timeinfo structure
  RTclock.fillByYMD(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);  // fill year, month and day from timeinfo structure
  RTclock.fillByHMS(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);              // fill hour, minute and second from timeinfo structure
  RTclock.setTime();                                                                  // set the filled time
  RTclock.startClock();                                                               // start the real-time clock

  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);

  delay(1000);
  digitalWrite(17, !LOW);
}

/* Loop what is shown on screen */
void loop() {
  drawlogo();
  showdate();
  showtime();
  showtemperature();
  showhumidity();
  showpressure();
  showaltitude();
  showxaccel();
  showyaccel();
}

/* Set a specific time and date, in case ntp time can't be reached */
void setSpecificTime(void) {
  RTclock.stopClock();
  RTclock.fillByYMD(2020,10,9);
  RTclock.fillByHMS(18,00,00);
  RTclock.fillDayOfWeek(FRI);
  RTclock.setTime();
  RTclock.startClock();
}

/* Logo bitmap */
#define LOGO_HEIGHT   58
#define LOGO_WIDTH    60
static const uint8_t logo_bmp[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x3f, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0x1f, 0x07, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0xff, 0x0f, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xcf, 0xc7, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x3f, 0xff, 0xcf, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xc0, 
  0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xc0, 
  0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xf0, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xf8, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x3f, 0xfe, 0x00, 
  0x00, 0x00, 0x00, 0x03, 0xf8, 0x3f, 0xff, 0x00, 0x00, 0x00, 0x00, 0x07, 0xf0, 0x3f, 0xff, 0x80, 
  0x00, 0x00, 0x00, 0x1f, 0xc0, 0x3f, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x3f, 0x80, 0x7c, 0xff, 0xc0, 
  0x00, 0x00, 0x00, 0x7f, 0x00, 0x78, 0xf3, 0xe0, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x78, 0xf1, 0xc0, 
  0x00, 0x00, 0x03, 0xf8, 0x00, 0xf0, 0xf0, 0x00, 0x00, 0x00, 0x07, 0xf0, 0x00, 0xf0, 0x78, 0x00, 
  0x00, 0x00, 0x0f, 0xe0, 0x00, 0xf0, 0x78, 0x00, 0x00, 0x00, 0x1f, 0xc0, 0x00, 0xf0, 0x78, 0x00, 
  0x00, 0x00, 0x3f, 0x80, 0x01, 0xe0, 0x3c, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x03, 0xe0, 0x3c, 0x00, 
  0x00, 0x00, 0xfe, 0x00, 0x03, 0xc0, 0x3c, 0x00, 0x00, 0x01, 0xfc, 0x00, 0x03, 0xc0, 0x3c, 0x00, 
  0x00, 0x07, 0xf0, 0x00, 0x03, 0xc0, 0x3c, 0x00, 0x00, 0x0f, 0xe0, 0x00, 0x03, 0x80, 0x18, 0x00, 
  0x00, 0x7f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x03, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x0f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x1f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x3f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* Show the logo and scroll it on screen */
void drawlogo(void) {
  display.clearDisplay();                       // clear the display buffer
  
  display.drawBitmap(                           // send a bitmap to the display buffer
    10,                                         // X position
    (display.height() - LOGO_HEIGHT) / 2,       // Y position
    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
    
  display.display();                            // update display to show what is on the buffer

  display.startscrollright(0x00, 0x0F);         // start scrolling objects on the display to the right
  delay(3000);                                  // wait 3 seconds
  display.stopscroll();                         // stop scrolling
}

/* Show current date on screen */
void showdate(void) {
  RTclock.getTime();                            // get the current time from the real-time clock
  
  display.clearDisplay();                       // clear the display buffer
  display.setTextSize(2);                       // set text size to a medium size (2:1 scale)
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(0,4);
  switch (RTclock.dayOfWeek) {                  // switch based on week day from the real-time clock
    case MON:
      display.print("Monday");
      break;
    case TUE:
      display.print("Tuesday");
      break;
    case WED:
      display.print("Wednesday");
      break;
    case THU:
      display.print("Thursday");
      break;
    case FRI:
      display.print("Friday");
      break;
    case SAT:
      display.print("Saturday");
      break;
    case SUN:
      display.print("Sunday");
      break;
  }

  display.setCursor(0,44);
  display.print(RTclock.year + 2000);         // print year from RTclock + 2000 to display buffer
  display.print("/");
  display.print(RTclock.month);               // print month from RTclock to display buffer
  display.print("/");
  display.print(RTclock.dayOfMonth);          // print day from RTclock to display buffer
  display.display();                          // update display to show what is on the buffer
  
  delay(3000);
}

/* Show current time on screen */
void showtime(void) {
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  
  int RThour;
  int RTminute;
  int RTsecond;
  
  int count = 0;
  while (count < 60) {                        // loop for 3 seconds (60x50ms)
    count++;
    
    RTclock.getTime();

    RThour = RTclock.hour;
    RTminute = RTclock.minute;
    RTsecond = RTclock.second;
  
    display.clearDisplay();                   // clear the display buffer
    
    display.setCursor(14,24);

    display.print(RThour);
    display.print(":");
    
    if (RTminute < 10) {                      // if the minutes have less than 2 digits
      display.print("0");                     // print to display buffer a 0 before
    }
    display.print(RTminute);
    display.print(":");
    
    if (RTsecond < 10) {                      // if the seconds have less than 2 digits
      display.print("0");                     // print to display buffer a 0 before
    }
    display.print(RTsecond);

    display.display();                        // update display to show what is on the buffer
    
    delay(50);                                // wait 50 ms
  }
}

/* Show current temperature on screen */
void showtemperature(void) {
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  
  int count = 0;
  while (count < 15) {                          // loop for 3 seconds (15x200ms)
    count++;
    
    display.clearDisplay();                     // clear the display buffer
    
    display.setCursor(0,4);
    display.print("Temp:");
  
    display.setCursor(40,44);
    display.print(si7021.readTemperature());    // print to display buffer the temperature value from Si7021 sensor
    display.print(" C");
    display.display();
    
    delay(200);                                 // wait 200 ms
  }
}

/* Show current humidity on screen */
void showhumidity(void) {
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  
  int count = 0;
  while (count < 15) {                          // loop for 3 seconds (15x200ms)
    count++;
    
    display.clearDisplay();                     // clear the display buffer
    
    display.setCursor(0,4);
    display.print("Humidity:");
  
    display.setCursor(40,44);
    display.print(si7021.readHumidity());       // print to display buffer the humidity value from Si7021 sensor
    display.print(" %");
    display.display();                          // update display to show what is on the buffer
    
    delay(200);                                 // wait 200 ms
  }
}

/* Show current air pressure on screen */
void showpressure(void) {
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  
  int count = 0;
  while (count < 15) {                          // loop for 3 seconds (15x200ms)
    count++;
    
    display.clearDisplay();                     // clear the display buffer
    
    display.setCursor(0,4);
    display.print("Pressure:");
  
    display.setCursor(0,34);
    display.print(bmp280.readPressure());       // print to display buffer the pressure value from BMP280 sensor

    display.setCursor(80,50);
    display.print(" Pa");
    display.display();                          // update display to show what is on the buffer
    
    delay(200);                                 // wait 200 ms
  }
}

/* Show current adjusted altitude on screen */
void showaltitude(void) {
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  
  int count = 0;
  while (count < 15) {                                // loop for 3 seconds (15x200ms)
    count++;
    
    display.clearDisplay();                           // clear the display buffer
    
    display.setCursor(0,4);
    display.print("Altitude:");
  
    display.setCursor(30,44);
    display.print(bmp280.readAltitude(seaLevelhPa));  // print to display buffer the altitude value from BMP280 sensor
    display.print(" m");
    display.display();                                // update display to show what is on the buffer
    
    delay(200);                                       // wait 200 ms
  }
}

/* Show current acceleration on the X axis */
void showxaccel(void) {
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);

  sensors_event_t accel;                              // create sensor event structure for acceleration
  sensors_event_t gyro;                               // create sensor event structure for gyroscope
  sensors_event_t temp;                               // create sensor event structure for temperature
  
  int count = 0;
  while (count < 60) {                                // loop for 3 seconds (60x50ms)
    count++;

    lsm6ds3.getEvent(&accel, &gyro, &temp);           // get a new normalized sensor event
    
    display.clearDisplay();                           // clear the display buffer
    
    display.setCursor(0,4);
    display.print("X accel:");
  
    display.setCursor(40,34);
    display.print(accel.acceleration.x);              // print to display buffer the X acceleration value from LSM6DS3 sensor
      
    display.setCursor(50,50);
    display.print(" m/s^2");
    display.display();                                // update display to show what is on the buffer
    
    delay(50);                                        // wait 50 ms
  }
}

/* Show current acceleration on the Y axis */
void showyaccel(void) {
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);

  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  
  int count = 0;
  while (count < 60) {
    count++;

    lsm6ds3.getEvent(&accel, &gyro, &temp);
    
    display.clearDisplay();
    
    display.setCursor(0,4);
    display.print("Y accel:");
  
    display.setCursor(40,34);
    display.print(accel.acceleration.y);
      
    display.setCursor(50,50);
    display.print(" m/s^2");
    display.display();
    
    delay(50);
  }
}
