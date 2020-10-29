#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_LSM6DS3.h>
#include <PCF85063TP.h>
#include <Adafruit_Si7021.h>

#define I2C_SDA 21
#define I2C_SCL 22
#define BMP_I2C_ADDR 0x76
#define LSM_I2C_ADDR 0x6B

Adafruit_BMP280 bmp280;
Adafruit_LSM6DS3 lsm6ds3;
PCD85063TP RTclock;
Adafruit_Si7021 si7021;

void setup() {
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.begin(115200);

  Serial.println("Discover32 Sensors test");

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
  RTclock.getTime();
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  lsm6ds3.getEvent(&accel, &gyro, &temp);

  Serial.println();
  
  Serial.print(RTclock.year + 2000);
  Serial.print("/");
  Serial.print(RTclock.month);
  Serial.print("/");
  Serial.print(RTclock.dayOfMonth);
  Serial.print(" ");
  switch (RTclock.dayOfWeek) {
    case MON:
      Serial.print("MON");
      break;
    case TUE:
      Serial.print("TUE");
      break;
    case WED:
      Serial.print("WED");
      break;
    case THU:
      Serial.print("THU");
      break;
    case FRI:
      Serial.print("FRI");
      break;
    case SAT:
      Serial.print("SAT");
      break;
    case SUN:
      Serial.print("SUN");
      break;
  }
  Serial.print(" ");
  Serial.print(RTclock.hour);
  Serial.print(":");
  Serial.print(RTclock.minute);
  Serial.print(":");
  Serial.println(RTclock.second);
  
  Serial.print("Temperature: ");
  Serial.print(si7021.readTemperature());
  Serial.print(" C");
  Serial.print("\tHumidity: ");
  Serial.print(si7021.readHumidity());
  Serial.println(" %");

  Serial.print("Pressure: ");
  Serial.print(bmp280.readPressure());
  Serial.print(" Pa");
  Serial.print("\tAltitude: ");
  Serial.print(bmp280.readAltitude(1013.25));
  Serial.println(" m");

  Serial.print("X accel: ");
  Serial.print(accel.acceleration.x);
  Serial.print(" m/s^2");
  Serial.print("\tY accel: ");
  Serial.print(accel.acceleration.y);
  Serial.println(" m/s^2");

  delay(1000);
}
