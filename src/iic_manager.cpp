#include <iic_manager.h>

void onReceive(int numBytes) {
  while (Wire.available()) {
    uint8_t cmd = Wire.read();
    if (cmd == CMD_BLINK) {
      blinkPending = true;
    }
  }
}

void onRequest() { Wire.write(SLAVE_ADDR); }

void setupI2C() {
  Wire.begin(SLAVE_ADDR);
  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);
}
