#include <Wire.h>
#include <PCF85063TP.h>

#define I2C_SDA 21
#define I2C_SCL 22

PCD85063TP RTclock;

void setup(void) {
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.begin(115200);

  Serial.println("PCF85063TP Real-Time Clock Calendar test");
  
  RTclock.begin();

  /* Set a specific time and date. */
  RTclock.stopClock();
  RTclock.fillByYMD(2020,10,8);
  RTclock.fillByHMS(17,00,00);
  RTclock.fillDayOfWeek(THU);
  RTclock.setTime();
  RTclock.startClock();
}

void loop(void) {
  Serial.println();
  
  RTclock.getTime();
  switch (RTclock.dayOfWeek)
  {
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
  Serial.print(RTclock.year+2000);
  Serial.print("/");
  Serial.print(RTclock.month);
  Serial.print("/");
  Serial.print(RTclock.dayOfMonth);
  Serial.print(" ");
  Serial.print(RTclock.hour);
  Serial.print(":");
  Serial.print(RTclock.minute);
  Serial.print(":");
  Serial.println(RTclock.second);

  delay(1000);
}
