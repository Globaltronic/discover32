#include <Adafruit_Si7021.h>

#define I2C_SDA 21
#define I2C_SCL 22

bool enableHeater = false;
uint8_t counter = 0;

Adafruit_Si7021 si7021;

void setup() {
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.begin(115200);

  Serial.println("Si7021 Humidity and Temperature Sensor test");
  
  if (!si7021.begin()) {
    Serial.println("Could not find a valid Si7021 sensor!");
    while (1);
  }
}

void loop() {
  Serial.println();
  
  Serial.print("Temperature: ");
  Serial.print(si7021.readTemperature());
  Serial.println(" C");
  Serial.print("Humidity: ");
  Serial.print(si7021.readHumidity());
  Serial.println(" %");
  
  delay(1000);

  /* Toggle heater enabled state every 30 seconds. */
  if (++counter == 30) {
    enableHeater = !enableHeater;
    si7021.heater(enableHeater);
    Serial.print("Heater State: ");
    if (si7021.isHeaterEnabled())
      Serial.println("ENABLED");
    else
      Serial.println("DISABLED");
       
    counter = 0;
  }
}
