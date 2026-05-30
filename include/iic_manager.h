#include <Arduino.h>
#include <Wire.h>
#include <led_manager.h>

// Change per module
#define SLAVE_ADDR 0x21

#define CMD_BLINK 0x01

void setupI2C();
