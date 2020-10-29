#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_LSM6DS3.h>
#include <PCF85063TP.h>
#include <Adafruit_Si7021.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <time.h>

#define I2C_SDA 21
#define I2C_SCL 22
#define BMP_I2C_ADDR 0x76
#define LSM_I2C_ADDR 0x6B
#define SSD_I2C_ADDR 0x3C
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_BMP280 bmp280;
Adafruit_LSM6DS3 lsm6ds3;
PCD85063TP RTclock;
Adafruit_Si7021 si7021;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const char* ssid     = "yourssid";
const char* password = "yourpassword";
bool led_enabled = false;
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;           // Greenwich Mean Time (GMT) offset
const int daylightOffset_sec = 0;       // 3600 during summer time (DST)

void setup() {
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.begin(115200);

  Serial.println("Discover Board Sensors OLED test");

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
  digitalWrite(17, HIGH);       // turn off led (logic inverted)

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,4);
  display.println("Connecting to");
  display.println(ssid);
  display.display();

  WiFi.begin(ssid, password);
  int count = 0;
  while (WiFi.status() != WL_CONNECTED) {
    led_enabled = !led_enabled;
    digitalWrite(17, led_enabled);
    if (count == 20) {
      WiFi.mode(WIFI_OFF);
      digitalWrite(17, HIGH);       // turn off led (logic inverted)
      
      display.println("Connection Failed");
      display.display();
      
      setSpecificTime();

      delay(2000);
      return;
    }
    count++;
    delay(500);
  }

  digitalWrite(17, LOW);       // turn on led (logic inverted)
  
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  display.println("Setting ntp time");
  display.display();

  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    display.clearDisplay();
    display.println("Failed to obtain time");
    display.display();

    WiFi.mode(WIFI_OFF);
    digitalWrite(17, HIGH);       // turn off led (logic inverted)
    
    setSpecificTime;

    delay(2000);
    return;
  }

  RTclock.stopClock();
  RTclock.fillDayOfWeek(timeinfo.tm_wday);
  RTclock.fillByYMD(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);
  RTclock.fillByHMS(timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  RTclock.setTime();
  RTclock.startClock();

  WiFi.mode(WIFI_OFF);

  delay(1000);
  digitalWrite(17, HIGH);       // turn off led (logic inverted)
}

void loop() {
  drawlogo();
  showday();
  showtime();
  showtemperature();
  showhumidity();
  showpressure();
  showaltitude();
  showxaccel();
  showyaccel();
}

void setSpecificTime(void) {
  RTclock.stopClock();
  RTclock.fillByYMD(2020,10,9);
  RTclock.fillByHMS(18,00,00);
  RTclock.fillDayOfWeek(FRI);
  RTclock.setTime();
  RTclock.startClock();
}

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


void drawlogo(void) {
  display.clearDisplay();
  
  display.drawBitmap(
    10,
    (display.height() - LOGO_HEIGHT) / 2,
    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
    
  display.display();

  display.startscrollright(0x00, 0x0F);
  delay(3000);
  display.stopscroll();
}

void showday(void) {
  RTclock.getTime();
  
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  
  display.setCursor(0,4);
  switch (RTclock.dayOfWeek) {
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
  display.print(RTclock.year+2000);
  display.print("/");
  display.print(RTclock.month);
  display.print("/");
  display.print(RTclock.dayOfMonth);
  display.display();
  
  delay(3000);
}

void showtime(void) {
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  
  int RThour;
  int RTminute;
  int RTsecond;
  
  int count = 0;
  while (count < 60) {
    count++;
    
    RTclock.getTime();

    RThour = RTclock.hour;
    RTminute = RTclock.minute;
    RTsecond = RTclock.second;
  
    display.clearDisplay();
    
    display.setCursor(14,24);

    display.print(RThour);
    display.print(":");
    
    if (RTminute < 10) {
      display.print("0");
    }
    display.print(RTminute);
    display.print(":");
    
    if (RTsecond < 10) {
      display.print("0");
    }
    display.print(RTsecond);

    display.display();
    
    delay(50);
  }
}

void showtemperature(void) {
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  
  int count = 0;
  while (count < 15) {
    count++;
    
    display.clearDisplay();
    
    display.setCursor(0,4);
    display.print("Temp:");
  
    display.setCursor(40,44);
    display.print(si7021.readTemperature());
    display.print(" C");
    display.display();
    
    delay(200);
  }
}

void showhumidity(void) {
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  
  int count = 0;
  while (count < 15) {
    count++;
    
    display.clearDisplay();
    
    display.setCursor(0,4);
    display.print("Humidity:");
  
    display.setCursor(40,44);
    display.print(si7021.readHumidity());
    display.print(" %");
    display.display();
    
    delay(200);
  }
}

void showpressure(void) {
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  
  int count = 0;
  while (count < 15) {
    count++;
    
    display.clearDisplay();
    
    display.setCursor(0,4);
    display.print("Pressure:");
  
    display.setCursor(0,34);
    display.print(bmp280.readPressure());

    display.setCursor(80,50);
    display.print(" Pa");
    display.display();
    
    delay(200);
  }
}

void showaltitude(void) {
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  
  int count = 0;
  while (count < 15) {
    count++;
    
    display.clearDisplay();
    
    display.setCursor(0,4);
    display.print("Altitude:");
  
    display.setCursor(30,44);
    display.print(bmp280.readAltitude(1013.25));
    display.print(" m");
    display.display();
    
    delay(200);
  }
}

void showxaccel(void) {
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
    display.print("X accel:");
  
    display.setCursor(40,34);
    display.print(accel.acceleration.x);
      
    display.setCursor(50,50);
    display.print(" m/s^2");
    display.display();
    
    delay(50);
  }
}

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
