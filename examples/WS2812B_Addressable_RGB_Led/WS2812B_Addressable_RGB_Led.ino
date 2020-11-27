#include <FastLED.h>

#define NUM_LEDS 1
#define DATA_PIN 13
#define INTERVAL 10   // delay between color transition iterations, in miliseconds

CRGB leds[NUM_LEDS];

void setup() {

  /* Add and configure the WS2812B Led. */
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  
  /* Turn the LED red, then green, then blue, then white, then off. */
  leds[0] = CRGB::Red;
  FastLED.show();
  delay(1000);

  leds[0] = CRGB::Green;
  FastLED.show();
  delay(1000);

  leds[0] = CRGB::Blue;
  FastLED.show();
  delay(1000);

  leds[0] = CRGB::White;
  FastLED.show();
  delay(1000);

  leds[0] = CRGB::Black;
  FastLED.show();
  delay(1000);
}

void loop() {

  /* Transition from Red to Green. */
  for (int i = 0; i <= 255; i++) {
    leds[0] = CRGB(255 - i, i, 0);
    FastLED.show();
    delay(INTERVAL);
  }

  /* Transition from Green to Blue. */
  for (int i = 0; i <= 255; i++) {
    leds[0] = CRGB(0, 255 - i, i);
    FastLED.show();
    delay(INTERVAL);
  }

  /* Transition from Blue to Red. */
  for (int i = 0; i <= 255; i++) {
    leds[0] = CRGB(i, 0, 255 - i);
    FastLED.show();
    delay(INTERVAL);
  }
}
