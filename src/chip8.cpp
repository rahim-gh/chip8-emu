#include <cstdio>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>

#include <unistd.h>

#include "../include/chip8.hpp"

const unsigned char chip8_fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8::Chip8() {
  // empty
}

Chip8::~Chip8() {
  // empty
}

void Chip8::initialize() {
  /// Before running the first emulation cycle, you will need to prepare your
  /// system state. Start clearing the memory and resetting the V to
  /// zero. While the Chip 8 doesn’t really have a BIOS or firmware, it does
  /// have a basic fontset stored in the memory. This fontset should be loaded
  /// in memory location 0x50 == 80 and onwards. More details about how the
  /// fontset works can be found at the end of this guide.
  /// Another important thing to remember is that the system expects the
  /// application to be loaded at memory location 0x200. This means that your
  /// program counter should also be set to this location.

  // Start of the application block in memory
  this->pc = 0x200;
  // Reset current opcode
  this->opcode = 0;
  // Reset index register
  this->I = 0;
  // Reset stack pointer
  this->sp = 0;

  // Clear display
  for (int i = 0; i < (64 * 32); ++i) {
    this->gfx[i] = 0;
  }

  // Clear stack
  for (int i = 0; i < 16; ++i) {
    this->stack[i] = 0;
  }

  // Clear V V0-VF
  for (int i = 0; i < 16; ++i) {
    this->V[i] = 0;
  }

  // Clear memory
  for (int i = 0; i < 4096; ++i) {
    this->memory[i] = 0;
  }

  // Load fontset
  for (int i = 0; i < 80; ++i)
    this->memory[i + 50] = chip8_fontset[i];

  // Reset key state
  for (int i = 0; i < 16; ++i) {
    key[i] = 0;
  }

  // Reset timers
  this->delay_timer = 0;
  this->sound_timer = 0;

  this->drawFlag = true;
}

void Chip8::emulateCycle() {
  // Fetch opcode
  opcode = memory[pc] << 8 | memory[pc + 1];

  // std::cout << "DEBUG: OPCODE " << std::hex << opcode << std::dec <<
  // std::endl;

  // Decode opcode
  switch (opcode & 0xF000) {
  case 0x0000:
    switch (opcode & 0x00FF) {
    case 0x00E0: // 0x00E0: Clear screen
      for (int i = 0; i < (64 * 32); ++i) {
        gfx[i] = 0;
      }
      pc += 2;
      break;
    case 0x00EE: // 0x00EE: Return from subroutine to address pulled from stack
      --sp;
      pc = stack[sp];
      pc += 2;
      break;
    default:
      printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
      break;
    }
    break;
  case 0x1000: // 0x1NNN: Jump to the address NNN
    pc = opcode & 0x0FFF;
    break;
  case 0x2000: // 0x2NNN: Jump to the address NNN, and save return address
    stack[sp] = pc;
    ++sp;
    pc = opcode & 0x0FFF;
    break;
  case 0x3000: // 0x3XNN: Skip next opcode if VX == NN
    if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
      pc += 4;
    } else {
      pc += 2;
    }
    break;
  case 0x4000: // 0x4XNN: Skip next opcode if VX != NN
    if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
      pc += 4;
    } else {
      pc += 2;
    }
    break;
  case 0x5000:
    switch (opcode & 0x000F) {
    case 0x0000: // 0x5XY0: Skip next opcode if VX == VY
      if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) {
        pc += 4;
      } else {
        pc += 2;
      }
      break;
    default:
      printf("Unknown opcode [0x5000]: 0x%X\n", this->opcode);
      break;
    }
    break;
  case 0x6000: // 0x6XNN: Set NN to VX
    V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
    pc += 2;
    break;
  case 0x7000: // 0x7XNN: Add NN to VX
    V[(opcode & 0x0F00) >> 8] += (opcode & 0x00FF);
    pc += 2;
    break;
  case 0x8000:
    switch (opcode & 0x000F) {
    case 0x0000: // 0x8XY0: set value of vY to vX
      V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
      pc += 2;
      break;
    case 0x0001: // 0x8XY1: set value of OR-bitwise of VY and VX to VX
      V[(opcode & 0x0F00) >> 8] =
          V[(opcode & 0x00F0) >> 4] | V[(opcode & 0x0F00) >> 8];
      V[0xF] = 0; // COSMAC based reset the VF
      pc += 2;
      break;
    case 0x0002: // 0x8XY2: set value of AND-bitwise of VY and VX to VX
      V[(opcode & 0x0F00) >> 8] =
          V[(opcode & 0x00F0) >> 4] & V[(opcode & 0x0F00) >> 8];
      V[0xF] = 0; // COSMAC based reset the VF
      pc += 2;
      break;
    case 0x0003: // 0x8XY3: set value of XOR-bitwise of VY and VX to VX
      V[(opcode & 0x0F00) >> 8] =
          V[(opcode & 0x00F0) >> 4] ^ V[(opcode & 0x0F00) >> 8];
      V[0xF] = 0; // COSMAC based reset the VF
      pc += 2;
      break;
    case 0x0004: // 0x8XY4: Add VY to VX, and set VF to 1 if there is overflow
      if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8])) {
        V[0xF] = 1; // carry
      } else {
        V[0xF] = 0;
      }
      V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
      pc += 2;
      break;
    case 0x0005: // 0x8XY5: Subtract VY from VX, and set VF to 0 if there is
                 // underflow
      if (V[(opcode & 0x00F0) >> 4] > (V[(opcode & 0x0F00) >> 8])) {
        V[0xF] = 0; // carry
      } else {
        V[0xF] = 1;
      }
      V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
      pc += 2;
      break;
    case 0x0006: // 0x8XY6: Set value of VY to VX, and shift one bit to right.
                 // and set VF to the bit shifted out
      // Store the one bit that would be shifted out
      V[0xF] = V[(opcode & 0x00F0) >> 4] & 0b1;
      V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] >> 1;
      pc += 2;
      break;
    case 0x0007: // 0x8XY7: Subtract VX from VY and store it in VX, and set VF
                 // to 0 if there is underflow
      if (V[(opcode & 0x0F00) >> 8] > (V[(opcode & 0x00F0) >> 4])) {
        V[0xF] = 0; // carry
      } else {
        V[0xF] = 1;
      }
      V[(opcode & 0x0F00) >> 8] =
          V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
      pc += 2;
      break;
    case 0x000E: // 0x8XYE: Set value of VY to VX, and shift one bit to left.
                 // and set VF to the bit shifted out
      // Storw the one bit that would be shifted out
      V[0xF] = V[(opcode & 0x00F0) >> 4] & 0b10000000;
      V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] << 1;
      pc += 2;
      break;
    default:
      printf("Unknown opcode [0x8000]: 0x%X\n", this->opcode);
      break;
    }
    break;
  case 0x9000: // 0x9XY0: Skip next opcode if VX != VY
    if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) {
      pc += 4;
    } else {
      pc += 2;
    }
    break;
  case 0xA000: // 0xANNN: Sets I to the address NNN
    // Execute opcode
    I = opcode & 0x0FFF;
    pc += 2;
    break;
  case 0xB000: // 0xBNNN: jump to address NNN + V0
    pc = (opcode & 0x0FFF) + V[0x0];
    break;
  case 0xC000: // 0xCXNN: Set random value masked with NN (AND-bitwise) to VX
    srand(time(0));
    V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF) & (rand() % 255);
    pc += 2;
    break;
  case 0xD000: // 0xDXYN: Draw sprite 8xN at X,Y position.
  {
    unsigned short x = V[(opcode & 0x0F00) >> 8];
    unsigned short y = V[(opcode & 0x00F0) >> 4];
    unsigned short height = opcode & 0x000F;
    unsigned short pixel;

    V[0xF] = 0;
    for (int yline = 0; yline < height; yline++) {
      pixel = memory[I + yline];
      for (int xline = 0; xline < 8; xline++) {
        if ((pixel & (0x80 >> xline)) != 0) {
          if (gfx[(x + xline + ((y + yline) * 64))] == 1)
            V[0xF] = 1; // collision detected
          gfx[x + xline + ((y + yline) * 64)] ^= 1;
        }
      }
    }

    drawFlag = true;
    pc += 2;
  } break;
  case 0xE000:
    switch (opcode & 0x00FF) {
    case 0x009E: // 0xEX9E: Skips the next instruction if the key stored in VX
                 // is pressed
      if (key[V[(opcode & 0x0F00) >> 8]] != 0) {
        pc += 4;
      } else {
        pc += 2;
      }
      break;
    case 0x00A1: // 0xEXA1: Skips the next instruction if the key stored in VX
                 // is NOT pressed
      if (key[V[(opcode & 0x0F00) >> 8]] == 0) {
        pc += 4;
      } else {
        pc += 2;
      }
      break;
    }
    break;
  case 0xF000:
    switch (opcode & 0x00FF) {
    case 0x0007: // 0xFX07
      V[(opcode & 0x0F00) >> 8] = delay_timer;
      pc += 2;
      break;
    case 0x000A: // 0xFX0A: A key press is awaited, and then stored in VX
    {
      int Vx = (opcode & 0x0F00) >> 8;

      if (key[0]) {
        V[Vx] = 0;
      } else if (key[1]) {
        V[Vx] = 1;
      } else if (key[2]) {
        V[Vx] = 2;
      } else if (key[3]) {
        V[Vx] = 3;
      } else if (key[4]) {
        V[Vx] = 4;
      } else if (key[5]) {
        V[Vx] = 5;
      } else if (key[6]) {
        V[Vx] = 6;
      } else if (key[7]) {
        V[Vx] = 7;
      } else if (key[8]) {
        V[Vx] = 8;
      } else if (key[9]) {
        V[Vx] = 9;
      } else if (key[10]) {
        V[Vx] = 10;
      } else if (key[11]) {
        V[Vx] = 11;
      } else if (key[12]) {
        V[Vx] = 12;
      } else if (key[13]) {
        V[Vx] = 13;
      } else if (key[14]) {
        V[Vx] = 14;
      } else if (key[15]) {
        V[Vx] = 15;
      } else {
        pc -= 2;
      }
    } break;
    case 0x0015: // 0xFX15
      delay_timer = V[(opcode & 0x0F00) >> 8];
      pc += 2;
      break;
    case 0x0018: // 0xFX18
      sound_timer = V[(opcode & 0x0F00) >> 8];
      pc += 2;
      break;
    case 0x001E: // 0xFX1E
      I += V[(opcode & 0x0F00) >> 8];
      pc += 2;
      break;
    case 0x0029: // 0xFX29: Sets I to the location of the sprite for the
                 // character in VX. Characters 0-F (in hexadecimal) are
                 // represented by a 4x5 font
      I = V[(opcode & 0x0F00) >> 8] * 0x5;
      pc += 2;
      break;
    case 0x0033: // 0xFX33: write the value of vX as BCD value at the addresses
                 // I, I+1 and I+2
      memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
      memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
      memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
      pc += 2;
      break;
    case 0x0055: // 0xFX55: write the content of v0 to vX at the memory pointed
      // to by I, I is incremented by X+1
      for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
        memory[I + i] = V[i];

      // On the original interpreter, when the operation is done, I = I + X + 1.
      I += ((opcode & 0x0F00) >> 8) + 1;
      pc += 2;
      break;
    case 0x0065: // 0xFX65: read the bytes from memory pointed to by I into the
                 // V v0 to vX, I is incremented by X+1

      for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
        V[i] = memory[I + i];

      // On the original interpreter, when the operation is done, I = I + X + 1.
      I += ((opcode & 0x0F00) >> 8) + 1;
      pc += 2;
      break;
    default:
      printf("Unknown opcode [0xF000]: 0x%X\n", this->opcode);
      break;
    }
    break;
  default:
    printf("Unknown opcode: 0x%X\n", this->opcode);
    break;
  }

  // Update timers
  if (delay_timer > 0)
    --delay_timer;

  if (sound_timer > 0) {
    if (sound_timer == 1)
      printf("BEEP!\n"); // TODO implement: support for sound
    --sound_timer;
  }

  // Simulate 60Hz (60 op per Second)
  usleep(1000000 / 60000000);
}

bool Chip8::loadGame(const std::string &gamePath) {
  try {
    // Open the file as a stream of binary and move the file pointer to the end
    std::ifstream gameFile(gamePath, std::ios::binary | std::ios::ate);

    if (gameFile.is_open()) {
      // Get size of file and allocate a buffer to hold the contents
      std::streampos size = gameFile.tellg();
      char *buffer = new char[size];

      // Go back to the beginning of the file and fill the buffer
      gameFile.seekg(0, std::ios::beg);
      gameFile.read(buffer, size);
      gameFile.close();

      // Load the ROM contents into the Chip8's memory, starting at 0x200
      for (long i = 0; i < size; ++i) {
        memory[0x200 + i] = buffer[i];
      }

      // Free the buffer
      delete[] buffer;
    }
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    return false;
    // exit(EXIT_FAILURE);
  }

  return true;
}

