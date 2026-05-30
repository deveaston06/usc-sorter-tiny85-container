#include <Arduino.h>
#include <iic_manager.h>
#include <led_manager.h>

void setup() {
  setupLED();
  setupI2C();

  blinkLED();
}

void loop() {
  if (blinkPending) {
    blinkPending = false;
    blinkLED();
  }
}
