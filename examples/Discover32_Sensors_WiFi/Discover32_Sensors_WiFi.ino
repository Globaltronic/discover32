
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_LSM6DS3.h>
#include <PCF85063TP.h>
#include <Adafruit_Si7021.h>
#include <WiFi.h>
#include <WebServer.h>
  
#define I2C_SDA 21
#define I2C_SCL 22
#define BMP_I2C_ADDR 0x76
#define LSM_I2C_ADDR 0x6B

Adafruit_BMP280 bmp280;
Adafruit_LSM6DS3 lsm6ds3;
PCD85063TP RTclock;
Adafruit_Si7021 si7021;
WebServer server(80);

const char* ssid     = "yourssid";
const char* password = "yourpassword";

void setup() {
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.begin(115200);

  Serial.println("Discover Board Sensors WiFi test");

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

  RTclock.begin();

  RTclock.stopClock();
  RTclock.fillByYMD(2020,10,9);
  RTclock.fillByHMS(18,00,00);
  RTclock.fillDayOfWeek(FRI);
  RTclock.setTime();
  RTclock.startClock();

}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
    }
    Serial.println("WiFi connected.");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    server.on("/", handleRoot);
    server.on("/api", handleAPI);
    server.onNotFound([]() {
      server.send(404, "text/plain", "Page not found");
    });
    server.begin();
  }
  server.handleClient();
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
      <title>Discover Board</title>\
    </head>\
    <body>\
      <h1>Discover Board Sensors</h1>\
      <p>%d/%d/%d %02d:%02d:%02d</p>\
      <p>Temperature: %.2f C</p>\
      <p>Humidity: %.1f %%</p>\
      <p>Pressure: %.1f Pa</p>\
      <p>Altitude: %.1f m</p>\
      <p>X acceleration: %.2f m/s^2</p>\
      <p>Y acceleration: %.2f m/s^2</p>\
    </body>\
  </html>",
  RTclock.year + 2000, RTclock.month, RTclock.dayOfMonth, RTclock.hour, RTclock.minute, RTclock.second,
    si7021.readTemperature(), si7021.readHumidity(), bmp280.readPressure(), bmp280.readAltitude(1013.25),
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
}"
, RTclock.year + 2000, RTclock.month, RTclock.dayOfMonth, RTclock.hour, RTclock.minute, RTclock.second,
    si7021.readTemperature(), si7021.readHumidity(), bmp280.readPressure(), bmp280.readAltitude(1013.25),
    accel.acceleration.x, accel.acceleration.y);

  server.send(200, "application/json", message);
}
