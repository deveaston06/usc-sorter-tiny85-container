// ─────────────────────────────────────────────────────────────
// led_manager.cpp — ATtiny85 Container
//
// Hardware:
//   Red LED
//   Green LED
//
// Behaviour:
//   Red  = default state (always on unless green active)
//   Green = commanded on, replaces red
//   Blink red = address just assigned
// ─────────────────────────────────────────────────────────────

#include <led_manager.h>

// ─────────────────────────────────────────────────────────────
// PUBLIC: INIT
// ─────────────────────────────────────────────────────────────
void led_init() {
  DDRB |= (1 << PIN_LED_RED) | (1 << PIN_LED_GREEN);
  RED_ON();
  GREEN_OFF();
}

// ─────────────────────────────────────────────────────────────
// PUBLIC: SET GREEN
// Turn on green, turn off red
// ─────────────────────────────────────────────────────────────
void led_setGreen() {
  RED_OFF();
  GREEN_ON();
}

// ─────────────────────────────────────────────────────────────
// PUBLIC: SET RED
// Turn on red, turn off green (return to default state)
// ─────────────────────────────────────────────────────────────
void led_setRed() {
  GREEN_OFF();
  RED_ON();
}

// ─────────────────────────────────────────────────────────────
// PUBLIC: BLINK RED ON ASSIGNMENT
// Briefly blink red to confirm address was assigned
// ─────────────────────────────────────────────────────────────
void led_blinkRedOnAssignment() {
  GREEN_OFF();
  for (uint8_t i = 0; i < BLINK_COUNT; i++) {
    RED_ON();
    _delay_ms(BLINK_ON_MS);
    RED_OFF();
    _delay_ms(BLINK_OFF_MS);
  }
  // return to default red state
  RED_ON();
}
