/*
 * I/O and MEMORY EMULATOR
 * By Tim Havelka, 2020
 * github.com/thavelka
 * 
 * Description:
 * A sketch that can emulate the I/O ports and RAM and ROM of a Z80-based computer.
 * Helpful for developing and testing the CPU, clock, and bus
 * Designed to work with Arduino Uno without any extra ICs at the tradeoff of only six bits of addressing.
 * This means support for 32 lines each of RAM and ROM. RAM is selected by setting A5 high.
 * 
 * Setup:
 * Address 0-5 to analog inputs 0-5
 * Data bus to digital pins 2-9
 * MREQ to digital pin 10
 * IORQ to digital pin 11
 * WR to digital pin 12
 * RD to digital pin 13
 * 
 * If the Z80 is not following the program, try slowing the clock so the Arduino can keep up.
 * Think hertz rather than megahertz.
 */

#define MEM_REQ 10
#define IO_REQ 11
#define WR 12
#define RD 13

/*
 * I/O checkout. Reads in byte from input port 0 and writes to output port 0. 
 */
//const byte ROM[] = {
//  0xDB, 0x00, // read port 0 in (1101 1011, 0000 0000)
//  0xD3, 0x00, // write to port 0 out (1101 0011, 0000 0000)
//  0xC3, 0x00, 0x00 // jmp to beginning (1100 0011, 0000 0000, 0000 0000)
//  };

/* 
 * Memory checkout. Reads a value from port 0, stores and retrieves from RAM, then writes to port 0.
 * In between writing and reading, the accumulator is set to zero and written to port 0 to prove the
 * value was stored and retreived successfully from memory.
 * Requires input and output at port 0
 */
byte ROM[] = {
  0xDB, 0x00,        // IN A, 0: Load val from port 0 into accumulator (11011011, 00000000)
  0x26, 0x00,        // LD H, 0x00: Load MSB of RAM start into register H (00100110, 00000000)
  0x2E, 0x20,        // LD L, 0x20: Load LSB of RAM start into register L (00101110, 00100000)
  0x77,              // LD (HL), A: Write val in accumulator to memory (01110111)
  0x3E, 0x00,        // LD A, 0: Load zero into accumulator (00111110, 00000000)
  0xD3, 0x00,        // OUT 0, A: Write accumulator to port 0 (11010011, 00000000)
  0x7E,              // LD A, (HL): Read value from memory into accumulator (01111110)
  0xD3, 0x00,        // OUT 0, A: Write accumulator to port 0 (11010011, 00000000)
  0xC3, 0x00, 0x00   // JP 0: Jump to start (11000011, 00000000, 00000000)
};

byte RAM[32] = {};
byte address = 0x00;
byte data = 0x00;
byte ioIn = B00000001;
byte ioOut = 0x00;

boolean memRd;
boolean memWr;
boolean ioRd;
boolean ioWr;

void setup() {
  DDRB = B00000000;
  DDRD = DDRD & B00000011;
  PORTD = B00000000;
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  
  Serial.begin(9600);
}

void loop() {
  // update all values
  address = PINC;
  if (!RD) {
    DDRB = B00000011;
    DDRD = DDRD | B11111100;
  } else {
    DDRB = B00000000;
    DDRD = DDRD & B00000011;
  }

  // Read from memory, write to bus
  boolean b = (!digitalRead(MEM_REQ) && !digitalRead(RD));
  if (b != memRd) {
    if (b) {
      boolean ram = address & B00100000;
      data = ram ? RAM[address & B00011111] : ROM[address];
      writeBus(data);
      Serial.print("Read byte ");
      Serial.print(data, HEX);
      Serial.print(" from address ");
      Serial.print(address, HEX);
      Serial.print(ram ? " (RAM)" : " (ROM)");
      Serial.print("\n");
    }
  }
  memRd = b;

  // Read from bus, write to memory
  b = (!digitalRead(MEM_REQ) && !digitalRead(WR));
  if (b != memWr) {
    if (b) {
      data = readBus();
      RAM[address & B00011111] = data;
      Serial.print("Wrote byte ");
      Serial.print(data, HEX);
      Serial.print(" to address ");
      Serial.print(address, HEX);
      Serial.print("\n");
    }
  }
  memWr = b;

  // Read from input port, write to bus
  b = (!digitalRead(IO_REQ) && !digitalRead(RD));
  if (b != ioRd) {
    if (b) {
      data = ioIn;
      writeBus(data);
      Serial.print("Read byte ");
      Serial.print(data, HEX);
      Serial.print(" from input port ");
      Serial.print(address, HEX);
      Serial.print("\n");
    }
  }
  ioRd = b;

  // Read from bus, write to output port
  b = (!digitalRead(IO_REQ) && !digitalRead(WR));
  if (b != ioWr) {
    if (b) {
      data = readBus();
      ioOut = data;
      Serial.print("Wrote byte ");
      Serial.print(data, HEX);
      Serial.print(" to output port ");
      Serial.print(address, HEX);
      Serial.print("\n");
    }
  }
  ioWr = b;
}

void writeInstruction(byte instruction) {
  PORTB = instruction >> 6;
  PORTD = instruction << 2;
}

byte readBus() {
  return PINB << 6 | PIND >>2;
}

void writeBus(byte d) {
  PORTB = d >> 6;
  PORTD = d << 2;
}
