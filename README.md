# USC-Sorter ATTiny85 Container

## Prerequisite

Ready a Arduino UNO with ISP Programmer and complete wiring to ATTiny85 by following this [guide](https://www.k4icy.com/program_attiny85.html)

Summary of First-time setting up a ATtiny85:

1. Burn bootloader via Arduino IDE (Tools > Burn Bootloader)
	to set fuses for 8MHz internal clock.
2. Call iic_writeUDID(uniqueSerial) ONCE before normal operation.
	Uncomment the block in setup() below, flash, then recomment.
	Each device must receive a different serial number.

## How to Program

### Use Platformio

First, Make sure you followed the [online guide](https://www.k4icy.com/program_attiny85.html). Then, Simply run `pio run -t upload`

### Use Arduino IDE

First, Make sure you followed the [online guide](https://www.k4icy.com/program_attiny85.html).
Then, run `./use_Arduino_IDE_Programmer.sh <PATH_TO_ARDUINO_SKETCH_DIRECTORY>` to translate into Arduino IDE files
Lastly, simply press Upload button.
