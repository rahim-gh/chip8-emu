# CHIP-8 Emulator

**CHIP-8** is an interpreted programming language, developed by **Joseph Weisbecker** on his 1802 microprocessor. It was initially used on the `COSMAC VIP` and `Telmac 1800`, which were 8-bit microcomputers made in the **mid-1970s**.

This repository is an Emulator writen in **C++** to emulate CHIP-8 games.

As this emulator is still in testing expect a lot of **uncompatibility issues**, and **crashes**.

It have been writen and tested for/on **linux only**.

## To Do

- [x] Can run some games
- [ ] Add sound support
- [ ] Fix the UI glitches/flickers
- [ ] Refactor the CHIP-8 methods
- [ ] Implement UI class using `GLUT` from `OpenGL`
- [ ] Write `CMake` file for compiling ease

## Getting Started

### Clone it

```sh
git clone https://github.com/rahim-gh/chip8-emu
```

### Compile it

You need:

- `GLUT` (`v3.6.0` used) (the package called `freeglut` on most distributions).
- `g++` or `clang++`, a C++ compiler.

```sh
g++ --std=c++14 \
    main.cpp src/chip8.cpp -o chip8_emulator \
    -Iinclude -lGL -lglut -lGLU \
    -Wall -Wextra -g -O0
```

### Run it

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

```md
    CHIP-8 Emu, A CHIP-8 games emulator writen in C++.
    Copyright (C) 2025 Guerrouf A.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
```
