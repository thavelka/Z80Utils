/*
 * Memory and output checkout with fibonacci. Repeatedly outputs operands and sum.
 * Requires at least three output ports.
 */
byte ROM[64] = {
  0x3E, 0x00,        // LD A, 0: Load zero into accumulator
  0x32, 0x00, 0x80,  // LD (0x8000), A: Load accumulator into RAM start
  0x3E, 0x01,        // LD A, 1: Load 1 into accumulator
  0x32, 0x01, 0x80,  // LD (0x8001), A: Load accumulator into RAM byte 1
  0x3E, 0x00,        // LD A, 0: Load 0 into accumulator
  0xD3, 0x00,        // OUT 0, A: Write accumulator to port 0
  0xD3, 0x01,        // OUT 0, A: Write accumulator to port 1
  0xD3, 0x02,        // OUT 0, A: Write accumulator to port 2
  0xD3, 0x03,        // OUT 0, A: Write accumulator to port 3
  // Loop start
  // Retrieve values from memory
  0x26, 0x80,        // LD H, 0x80: Load MSB of RAM start into register H
  0x2E, 0x00,        // LD L, 0: Load LSB of RAM start into register L
  0x7E,              // LD A, (HL): Read value from memory into accumulator
  0xD3, 0x03,        // OUT 0, A: Write accumulator to port 3
  0x47,              // LD B, A: Load value from accumulator into register B
  0x26, 0x80,        // LD H, 0x80: Load MSB of RAM byte 1 into register H
  0x2E, 0x01,        // LD L, 0: Load LSB of RAM byte 1 into register L
  0x7E,              // LD A, (HL): Read value from memory into accumulator
  0xD3, 0x02,        // OUT 0, A: Write accumulator to port 2
  0x4F,              // LD C, A: Load value from accumulator into register C
  // Add new total and display
  0x80,              // ADD A, B: Add value from register B to accumulator
  0xD3, 0x00,        // OUT 0, A: Write accumulator to port 0
  // Save registers to memory
  0x26, 0x80,        // LD H, 0x80: Load MSB of RAM start into register H
  0x2E, 0x00,        // LD L, 0: Load LSB of RAM start into register L
  0x71,              // LD (HL), B: Load value from register C into memory
  0x26, 0x80,        // LD H, 0x80: Load MSB of RAM byte 1 into register H
  0x2E, 0x01,        // LD L, 0: Load LSB of RAM byte 1 into register L
  0x77,              // LD (HL), A: Load value from accumulator into memory
  // Jump to start of loop
  0xC3, 0x14, 0x00  // JP 14: Jump to line 20
};
