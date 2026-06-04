#include <EEPROM.h>
#include <Wire.h>
#include <avr/io.h>
#include <led_manager.h>

// ── Pin definitions ───────────────────────────────────────────
#define PIN_ALERT PB1 // open-drain output, active low
#define PIN_SDA PB0
#define PIN_SCL PB2

// ── I2C addresses ─────────────────────────────────────────────
#define ADDR_ARP_DEFAULT 0x55 // all unresolved devices listen here
#define ADDR_UNASSIGNED 0x55

// ── Commands ──────────────────────────────────────────────────
#define CMD_GET_UDID 0x01
#define CMD_ASSIGN_ADDR 0x02
#define CMD_PREPARE_ARP 0x03
#define CMD_LED_GREEN 0x10
#define CMD_LED_RED 0x11

// ── EEPROM layout ─────────────────────────────────────────────
// 0-1:  protocolVersion (uint16)
// 2-3:  deviceType      (uint16)
// 4-7:  serialNumber    (uint32) — written uniquely at flash time
// 8:    capabilities    (uint8)
// 9:    AV flag         (uint8)  — 0x01 = resolved
// 10:   assigned addr   (uint8)
#define EE_UDID_START 0
#define EE_AV_FLAG 9
#define EE_ASSIGNED_ADDR 10
#define UDID_SIZE 9

// ── UDID device type ──────────────────────────────────────────
#define DEVICE_TYPE_ATTINY85 0x0085
#define PROTOCOL_VERSION 0x0001
#define CAPABILITIES 0x01 // I2C slave only

void iic_init();
void iic_update();
void iic_writeUDID(uint32_t serialNumber);
