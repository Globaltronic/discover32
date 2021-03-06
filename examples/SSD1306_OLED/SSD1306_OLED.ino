#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define I2C_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

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
 
void setup() {
  Serial.begin(115200);

  Serial.println("SSD1306 OLED Screen test");
 
  if(!display.begin(SSD1306_SWITCHCAPVCC, I2C_ADDR)) {
    Serial.println("Could not find a valid SSD1306 screen!");
    while (1);
  }

  display.clearDisplay();
  
  drawlogo();
  delay(2000);
 
}
 
void loop() {
  display.clearDisplay();

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,4);
  display.print("Hello, world!");
  display.display();
  delay(2000);

  display.setTextSize(1);
  display.setCursor(0,44);
  display.print("Globaltronic Discover Board");
  display.display();
  delay(2000);

  drawlogo();
  delay(1000);

  display.startscrollright(0x00, 0x0F);
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.startscrollright(0x00, 0x0F);
  delay(1000);
  display.stopscroll();
  delay(2000);
}


void drawlogo(void) {
  display.clearDisplay();

  display.drawBitmap(
    (display.width()  - LOGO_WIDTH ) / 2,
    (display.height() - LOGO_HEIGHT) / 2,
    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, 1);
  display.display();
}
