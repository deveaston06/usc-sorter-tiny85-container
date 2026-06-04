# USC-Sorter ATTiny85 Container

## Prerequisite

Ready a Arduino UNO with ISP Programmer and complete wiring to ATTiny85 by following this [guide](https://www.k4icy.com/program_attiny85.html)

First-time setup:

1. Burn bootloader via Arduino IDE (Tools > Burn Bootloader)
	to set fuses for 8MHz internal clock.
2. Call iic_writeUDID(uniqueSerial) ONCE before normal operation.
	Uncomment the block in setup() below, flash, then recomment.
	Each device must receive a different serial number.
