# CHIP-8 Emulator

**CHIP-8** is an 8-bit assembly for the `COSMAC VIP`, This repository is an emulator writen in **C++** to emulate CHIP-8 games.

As this emulator is still in testing you should expect **bugs**, **errors**, **uncompatibility**, and even **crashes**.

It have been writen and tested for/on **linux only**.

## To Do

- [x] Can run some games
- [ ] Add sound support
- [ ] Fix the UI glitches/flickers
- [ ] Refactor the CHIP-8 methods
- [ ] Implement UI class using `GLUT` from `OpenGL`
- [ ] Write `CMake` file for compiling ease

## Getting Started

- Clone it

```sh
git clone https://github.com/rahim-gh/chip8-emu
```

- Compile it

```sh
g++ main.cpp src/chip8.cpp -o chip8_emulator -Iinclude -lGL -lglut -lGLU -Wall -Wextra
```

- Run it

You can choose one of the games from `games/` directory, or install one from [CHIP-8 Archive](https://archive.org/details/chip-8-games).

```sh
Usage: ./chip8_emulator <rom_file>
Controls:
  1 2 3 4    ->  1 2 3 C
  Q W E R    ->  4 5 6 D
  A S D F    ->  7 8 9 E
  Z X C V    ->  A 0 B F
    ESC      ->   Exit

```

The keys have been already re-mapped from *ORIGINAL* to *ALTERNATIVE*

## License

NONE
