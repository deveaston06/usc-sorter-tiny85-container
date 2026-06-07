// ─────────────────────────────────────────────────────────────
// iic_manager.cpp — ATtiny85 Container
// ATTinyCore Wire library (PlatformIO)
//
// Responsibilities:
//   - UDID storage and retrieval from EEPROM
//   - Response to GET_UDID with bus arbitration awareness
//   - Response to ASSIGN_ADDR
//   - Response to PREPARE_ARP
//   - ALERT line assertion and release
//   - Relay LED commands to led_manager
// ─────────────────────────────────────────────────────────────

#include <iic_manager.h>

// ── Module state ──────────────────────────────────────────────
static uint8_t udid[UDID_SIZE];
static bool addressResolved = false;
static uint8_t assignedAddr = ADDR_ARP_DEFAULT;

// ── Receive buffer ────────────────────────────────────────────
// kept small to protect SRAM
#define RX_BUF_SIZE 12
static volatile uint8_t rxBuf[RX_BUF_SIZE];
static volatile uint8_t rxLen = 0;
static volatile bool rxPending = false;
static volatile bool txUdidReady = false;

// ─────────────────────────────────────────────────────────────
// ALERT LINE HELPERS
// ─────────────────────────────────────────────────────────────
static void alert_assert() {
  PORTB &= ~(1 << PIN_ALERT); // ensure output value is low
  DDRB |= (1 << PIN_ALERT);   // drive low
}

static void alert_release() {
  DDRB &= ~(1 << PIN_ALERT);  // input (Hi-Z)
  PORTB &= ~(1 << PIN_ALERT); // disable pull-up
}

// ─────────────────────────────────────────────────────────────
// UDID HELPERS
// ─────────────────────────────────────────────────────────────
static void udid_load() {
  for (uint8_t i = 0; i < UDID_SIZE; i++) {
    udid[i] = EEPROM.read(EE_UDID_START + i);
  }
}

// Call once at factory flash time to write a unique serial
// into EEPROM. All other UDID fields are constants.
// In production: replace serialNumber with a unique value per device.
void iic_writeUDID(uint32_t serialNumber) {
  uint16_t pv = PROTOCOL_VERSION;
  uint16_t dt = DEVICE_TYPE_ATTINY85;

  EEPROM.write(EE_UDID_START + 0, (uint8_t)(pv >> 8));
  EEPROM.write(EE_UDID_START + 1, (uint8_t)(pv));
  EEPROM.write(EE_UDID_START + 2, (uint8_t)(dt >> 8));
  EEPROM.write(EE_UDID_START + 3, (uint8_t)(dt));
  EEPROM.write(EE_UDID_START + 4, (uint8_t)(serialNumber >> 24));
  EEPROM.write(EE_UDID_START + 5, (uint8_t)(serialNumber >> 16));
  EEPROM.write(EE_UDID_START + 6, (uint8_t)(serialNumber >> 8));
  EEPROM.write(EE_UDID_START + 7, (uint8_t)(serialNumber));
  EEPROM.write(EE_UDID_START + 8, CAPABILITIES);
}

// ─────────────────────────────────────────────────────────────
// COMPARE UDID
// returns true if received buffer matches stored UDID
// ─────────────────────────────────────────────────────────────
static bool udid_matches(const volatile uint8_t *buf) {
  for (uint8_t i = 0; i < UDID_SIZE; i++) {
    if (buf[i] != udid[i])
      return false;
  }
  return true;
}

// ─────────────────────────────────────────────────────────────
// STAGGER HELPER
// Uses 4 serial bytes (UDID[4..7]) and returns delay in milliseconds
// ─────────────────────────────────────────────────────────────
static uint16_t stagger_delay_ms() {
  uint16_t serial_low = ((uint16_t)udid[6] << 8) | udid[7];

  // 16-bit serial gives 65536 possible values
  // Map to SLOT_MS (4ms) slots within a max window

  uint16_t slot = serial_low % MAX_SLOTS;
  return (slot + 1) * SLOT_MS;
}

// ─────────────────────────────────────────────────────────────
// WIRE CALLBACKS (ISR context — keep short, no blocking)
// ─────────────────────────────────────────────────────────────
static void onReceive(int numBytes) {
  rxLen = 0;
  while (Wire.available() && rxLen < RX_BUF_SIZE) {
    rxBuf[rxLen++] = Wire.read();
  }
  rxPending = true;
}

// Called when master requests data from this slave
// If AV=0, respond with UDID for arbitration
// If AV=1, respond with assigned address as heartbeat
static void onRequest() {
  if (!addressResolved) {
    // transmit UDID — open-drain arbitration resolves winner
    Wire.write(udid, UDID_SIZE);
  } else {
    Wire.write(assignedAddr);
  }
}

// ─────────────────────────────────────────────────────────────
// PROCESS RECEIVED COMMAND
// called from iic_update() in main loop context
// ─────────────────────────────────────────────────────────────
static void processCommand() {
  if (rxLen == 0)
    return;

  uint8_t cmd = rxBuf[0];

  switch (cmd) {

  // ── GET_UDID: master requests UDID from unresolved devices ─
  case CMD_GET_UDID:
    // response handled in onRequest()
    // if AV=1 we ignore (onRequest sends addr instead)
    break;

  // ── ASSIGN_ADDR: [UDID 9 bytes][new_addr 1 byte] ──────────
  case CMD_ASSIGN_ADDR:
    if (rxLen >= UDID_SIZE + 2 && !addressResolved) {
      if (udid_matches(&rxBuf[1])) {
        // we won arbitration — accept assigned address
        assignedAddr = rxBuf[UDID_SIZE + 1];
        addressResolved = true;

        // persist to EEPROM
        EEPROM.write(EE_ASSIGNED_ADDR, assignedAddr);

        // restart Wire on new address
        Wire.end();
        Wire.begin(assignedAddr);
        Wire.onReceive(onReceive);
        Wire.onRequest(onRequest);

        // release ALERT — we are resolved
        alert_release();

        led_blinkRedOnAssignment();
      } else {
        delay(20);
        alert_assert();
      }
    }
    break;

  // ── PREPARE_ARP: reset AV flag, re-enter discovery ────────
  case CMD_PREPARE_ARP:
    addressResolved = false;

    // move back to default ARP address
    Wire.end();
    Wire.begin(ADDR_ARP_DEFAULT);
    Wire.onReceive(onReceive);
    Wire.onRequest(onRequest);
    break;

  // ── LED COMMANDS ───────────────────────────────────────────
  case CMD_LED_GREEN:
    led_setGreen();
    break;

  case CMD_LED_RED:
    led_setRed();
    break;

  default:
    break;
  }
}

// ─────────────────────────────────────────────────────────────
// PUBLIC: INIT
// ─────────────────────────────────────────────────────────────
void iic_init() {
  alert_release();

  // load UDID from EEPROM
  udid_load();

  addressResolved = false;

  // silent during stagger — do not listen yet
  uint8_t stagger = udid[7];
  delay(2000 + (uint16_t)stagger * 10);

  Wire.begin(ADDR_ARP_DEFAULT);
  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);
  alert_assert();
}

// ─────────────────────────────────────────────────────────────
// PUBLIC: UPDATE (call every loop iteration)
// ─────────────────────────────────────────────────────────────
void iic_update() {
  if (rxPending) {
    rxPending = false;
    processCommand();
  }
}

// ─────────────────────────────────────────────────────────────
// PUBLIC: GETTERS
// ─────────────────────────────────────────────────────────────
bool iic_isResolved() { return addressResolved; }
uint8_t iic_getAddr() { return assignedAddr; }
