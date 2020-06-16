/*
 * SIMPLE Z80 MEMORY EMULATOR
 * By Tim Havelka, 2020
 * github.com/thavelka
 * 
 * Description:
 * A sketch that can emulate the RAM and ROM of a Z80-based computer.
 * Helpful for developing and testing the CPU and I/O ports.
 * Designed to work with Arduino Uno without any extra ICs at the tradeoff of only six bits of addressing.
 * This means support for 64 lines each of RAM and ROM. RAM is selected by setting A15 high.
 * 
 * Setup:
 * Address 0-5 to analog inputs 0-5
 * Data bus to digital pins 2-9
 * MREQ to digital pin 10
 * WR to digital pin 11
 * A15 to digital pin 12 (selects RAM when high)
 * 
 * If the Z80 is not following the program, try slowing the clock so the Arduino can keep up.
 * Think hertz rather than megahertz.
 */

#define MREQ 10
#define WR 11
#define RAM_SEL 12

//================= TEST PROGRAMS, UNCOMMENT ONE TO SELECT =====================

/*
 * I/O checkout. Reads in byte from input port 0 and writes to output port 0. 
 */
//byte ROM[64] = {
//  0xDB, 0x00,       // IN A, 0: Load val from port 0 into accumulator (11011011, 00000000)
//  0xD3, 0x00,       // OUT 0, A: Write accumulator to port 0 (11010011, 00000000)
//  0xC3, 0x00, 0x00  // JP 0: Jump to start (11000011, 00000000, 00000000)
//};

/* 
 * I/O and math checkout. Reads in a value from input port 0, then repeatedly increments accumulator 
 * and writes to output port 0. 
 */
//byte ROM[64] = {
//  0xDB, 0x00,       // IN A, 0: Load val from port 0 into accumulator (11011011, 00000000)
//  0xD3, 0x00,       // OUT 0, A: Write accumulator to port 0 (11010011, 00000000)
//  0xC6, 0x01,       // ADD A, 1: Add 1 to accumulator (1100 0110, 0000 0001)
//  0xC3, 0x02, 0x00  // JP 2: Jump to line 2 (1100 0011, 0000 0010, 0000 0000)
//};

/* 
 * Memory checkout. Reads a value from port 0, stores and retrieves from RAM, then writes to port 0.
 * In between writing and reading, the accumulator is set to zero and written to port 0 to prove the
 * value was stored and retreived successfully from memory.
 */
byte ROM[64] = {
  0xDB, 0x00,        // IN A, 0: Load val from port 0 into accumulator (11011011, 00000000)
  0x26, 0x80,        // LD H, 0x80: Load MSB of RAM start into register H (00100110, 10000000)
  0x2E, 0x00,        // LD L, 0: Load LSB of RAM start into register L (00101110, 00000000)
  0x77,              // LD (HL), A: Write val in accumulator to memory (01110111)
  0x3E, 0x00,        // LD A, 0: Load zero into accumulator (00111110, 00000000)
  0xD3, 0x00,        // OUT 0, A: Write accumulator to port 0 (11010011, 00000000)
  0x7E,              // LD A, (HL): Read value from memory into accumulator (01111110)
  0xD3, 0x00,        // OUT 0, A: Write accumulator to port 0 (11010011, 00000000)
  0xC3, 0x00, 0x00   // JP 0: Jump to start (11000011, 00000000, 00000000)
};
//==============================================================================

// 64 bytes of emulated ram
byte RAM[64] = {};

// Only used to prevent spamming console
byte lastAddress = 0xFF;
boolean lastOpWr = false;

void setup() {
  // Configure port directions
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
  // Read in 6 bit address
  byte address = PINC;
  boolean ramSelect = digitalRead(RAM_SEL);

  // Determine if read, write, or neither
  boolean readMode = digitalRead(WR) && !digitalRead(MREQ);
  boolean writeMode = !digitalRead(WR) && !digitalRead(MREQ) && ramSelect;

  // Configure port directions
  DDRB = readMode ? B00000011 : B00000000;
  DDRD = readMode ? (DDRD | B11111100) : (DDRD & B00000011);
  
  if (readMode) {
    // Write byte from memory to ports
    byte data = ramSelect ? RAM[address] : ROM[address];
    PORTB = data >> 6;
    PORTD = data << 2;

    // Log read to console
    if (address != lastAddress || lastOpWr) { 
      lastAddress = address;
      lastOpWr = false;
      Serial.print("Read byte ");
      Serial.print(data, HEX);
      Serial.print(" from address ");
      Serial.print(address, HEX);
      Serial.print(ramSelect ? " (RAM)" : " (ROM)");
      Serial.print("\n");
    }
  } else if (writeMode) {
    // Save byte in RAM
    byte data = PINB << 6 | PIND >>2;
    RAM[address] = data;

    // Log write to console
    if (true && !lastOpWr) {
      lastAddress = address;
      lastOpWr = true;
      Serial.print("Wrote byte ");
      Serial.print(data, HEX);
      Serial.print(" to address ");
      Serial.print(address, HEX);
      Serial.print("\n");
    }
  } else {
    // Reset for logging purposes
    lastAddress = 0xFF;
    lastOpWr = false;
  }
}
