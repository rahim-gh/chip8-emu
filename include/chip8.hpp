#include <random>
#include <string>

class Chip8 {
private:
  std::mt19937 rng;

public:
  Chip8();
  ~Chip8();

  /// Flag to indicate drawing is requested or not
  bool drawFlag;

  /// The Chip 8 has 35 opcodes which are all two bytes long.
  unsigned short opcode;

  /// The Chip 8 has 4K memory in total
  ///
  /// 0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
  ///
  /// 0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
  ///
  /// 0x200-0xFFF - Program ROM and work RAM
  unsigned char memory[4096];

  /// The Chip 8 has 15 8-bit general purpose registers named V0,V1 up to VE.
  /// The 16th register is used for the ‘carry flag’.
  unsigned char V[16];

  /// Index register (0x000 to 0xFFF)
  unsigned short I;

  /// Program Counter (0x000 to 0xFFF).
  ///
  /// Every instruction is 2 bytes long, you need to increment the program
  /// counter by two after every executed opcode. This is true unless you jump
  /// to a certain address in the memory or if you call a subroutine (in which
  /// case you need to store the program counter in the stack). If the next
  /// opcode should be skipped, increase the program counter by four.
  unsigned short pc;

  /// The graphics of the Chip 8 are black and white and the screen has a total
  /// of 2048 pixels (64 x 32). This array that hold the pixel state (1 or 0)
  unsigned char gfx[64 * 32];

  /// Count at 60 Hz (Execute 60 opcodes in one second). When set above zero it
  /// will count down to zero.
  unsigned char delay_timer;

  /// Count at 60 Hz (Execute 60 opcodes in one second). When set above zero it
  /// will count down to zero. The system’s buzzer sounds whenever the sound
  /// timer reaches zero.
  unsigned char sound_timer;

  /// The system has 16 levels of stack
  /// That allow the program to register the location before jump to a certain
  /// address or call a subroutine
  unsigned short stack[16];

  /// Stack Pointer
  /// To remember which level of the stack is used.
  unsigned short sp;

  /// Chip 8 has a HEX based keypad (0x0-0xF), this used to store the state of
  /// the keys
  unsigned char key[16];

  /// Load game into the memory starting from 0x200 (512) to 0xFFF (4095)
  /// Return false in case of failure in loading the game
  bool loadGame(const std::string &gamePath);

  /// Initialize registers and memory once.
  ///
  /// Clearing the memory and resetting the registers to zero.
  void initialize();

  /// Every cycle, the method emulateCycle is called which emulates one cycle of
  /// the Chip 8 CPU. During this cycle, the emulator will Fetch, Decode and
  /// Execute one opcode.
  void emulateCycle();
};

