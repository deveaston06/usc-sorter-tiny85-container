#include <Arduino.h>
#include <avr/io.h>
#include <util/delay.h>

#define PIN_LED_RED PB3
#define PIN_LED_GREEN PB4

#define BLINK_ON_MS 150
#define BLINK_OFF_MS 150
#define BLINK_COUNT 3

// ── Active low helpers ────────────────────────────────────────
#define RED_ON() PORTB &= ~(1 << PIN_LED_RED)
#define RED_OFF() PORTB |= (1 << PIN_LED_RED)
#define GREEN_ON() PORTB &= ~(1 << PIN_LED_GREEN)
#define GREEN_OFF() PORTB |= (1 << PIN_LED_GREEN)

void led_init();
void led_setGreen();
void led_setRed();
void led_blinkRedOnAssignment();
