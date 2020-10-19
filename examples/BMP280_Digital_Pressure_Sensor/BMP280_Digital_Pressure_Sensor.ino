#include <Wire.h>
#include <Adafruit_BMP280.h>

#define I2C_SDA 21
#define I2C_SCL 22
#define I2C_ADDR 0x76

Adafruit_BMP280 bmp280;

void setup() {
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.begin(115200);
  
  Serial.println("BMP280 Digital Pressure Sensor test");

  if (!bmp280.begin(I2C_ADDR, BMP280_CHIPID)) {
    Serial.println("Could not find a valid BMP280 sensor!");
    while (1);
  }

  /* Default settings from datasheet. */
  bmp280.setSampling(Adafruit_BMP280::MODE_NORMAL,  // Operating Mode
                  Adafruit_BMP280::SAMPLING_X2,     // Temp. oversampling
                  Adafruit_BMP280::SAMPLING_X16,    // Pressure oversampling
                  Adafruit_BMP280::FILTER_X16,      // Filtering
                  Adafruit_BMP280::STANDBY_MS_500); // Standby time
}

void loop() {
  Serial.println();
  
  Serial.print("Temperature: ");
  Serial.print(bmp280.readTemperature());
  Serial.println(" C");

  Serial.print("Pressure: ");
  Serial.print(bmp280.readPressure());
  Serial.println(" Pa");

  Serial.print("Altitude: ");
  Serial.print(bmp280.readAltitude(1013.25)); // Adjusted to local forecast!
  Serial.println(" m");

  delay(1000);
}
