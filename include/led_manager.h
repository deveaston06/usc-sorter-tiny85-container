#include <Arduino.h>

#define LED_PIN 3

#define BLINK_COUNT 3
#define BLINK_ON_MS 150
#define BLINK_OFF_MS 150

extern volatile bool blinkPending;

void setupLED();
void blinkLED();
