# PIC18F Keyboard Driver
This is a minimal keyboard driver for CH375 running on PIC18F4520.

This is a part of [picos](https://github.com/beautiful-fruit/picos), which is our final project of microcontroller class.

## Build
```
git clone https://github.com/rota1001/pic18f-keyboard-driver.git
cd pic18f-keyboard-driver
wget https://packs.download.microchip.com/Microchip.PIC18Fxxxx_DFP.1.7.171.atpack
unzip Microchip.PIC18Fxxxx_DFP.1.7.171.atpack -d dfp
make
```
## Usage
First, connect the RX and TX to the USB to serial module.
|PIC18F|USB-to-serial|
|--|--|
|RC7|TX|
|RC6|RX|
|GND|GND|

Second, connect the following pins to CH375.
|PIC18F|CH375|
|--|--|
|RD0|D0|
|RD1|D1|
|RD2|D2|
|RD3|D3|
|RD4|D4|
|RD5|D5|
|RD6|D6|
|RD7|D7|
|RA0|A0|
|RA1|WR|
|RA2|RD|
|RB0|INT|
|GND|GND|
|GND|RST|
|GND|CS|
|VDD|5V|

