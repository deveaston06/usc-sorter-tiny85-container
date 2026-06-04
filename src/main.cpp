#include <Arduino.h>
#include <iic_manager.h>
#include <led_manager.h>

// ─────────────────────────────────────────────────────────────
// SETUP
// ─────────────────────────────────────────────────────────────
void setup() {
  led_init();

  // ── FIRST TIME ONLY: write unique UDID to EEPROM ──────────
  // Uncomment, flash with unique serial per device, then recomment.
  // iic_writeUDID(0x00000001); // device 1
  // iic_writeUDID(0x00000002); // device 2
  // iic_writeUDID(0x00000003); // device 3

  iic_init();
  // iic_init() asserts ALERT if no stored address found,
  // and triggers RP2040 to begin enumeration automatically.
}

// ─────────────────────────────────────────────────────────────
// LOOP
// ─────────────────────────────────────────────────────────────
void loop() {
  iic_update();
  // TinyWireS requires stop check if using TinyWireS library.
  // ATTinyCore Wire handles this internally — no extra call needed.
}
