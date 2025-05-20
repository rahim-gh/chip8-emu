#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>

#include <GL/freeglut_std.h>
#include <GL/glut.h>

#include "include/chip8.hpp"

// Configuration constants
constexpr int CHIP8_SCREEN_WIDTH = 64;
constexpr int CHIP8_SCREEN_HEIGHT = 32;
constexpr int INITIAL_SCALE = 15;

// Global state
class EmulatorState {
public:
  Chip8 chip8;
  int window_scale = INITIAL_SCALE;
  int display_width = CHIP8_SCREEN_WIDTH * INITIAL_SCALE;
  int display_height = CHIP8_SCREEN_HEIGHT * INITIAL_SCALE;

  void updateDisplaySize(int width, int height) {
    display_width = width;
    display_height = height;
    window_scale =
        std::min(width / CHIP8_SCREEN_WIDTH, height / CHIP8_SCREEN_HEIGHT);
    if (window_scale == 0)
      window_scale = 1;
  }
};

static EmulatorState emulator;

// Input mapping: Maps PC keyboard to Chip-8 keypad
// Chip-8 layout:    PC keyboard layout:
// 1 2 3 C           1 2 3 4
// 4 5 6 D           Q W E R
// 7 8 9 E           A S D F
// A 0 B F           Z X C V
void handleKeyPress(unsigned char key, bool pressed) {
  const int state = pressed ? 1 : 0;

  switch (std::tolower(key)) {
  // Row 1
  case '1':
    emulator.chip8.key[0x1] = state;
    break;
  case '2':
    emulator.chip8.key[0x2] = state;
    break;
  case '3':
    emulator.chip8.key[0x3] = state;
    break;
  case '4':
    emulator.chip8.key[0xC] = state;
    break;

  // Row 2
  case 'q':
    emulator.chip8.key[0x4] = state;
    break;
  case 'w':
    emulator.chip8.key[0x5] = state;
    break;
  case 'e':
    emulator.chip8.key[0x6] = state;
    break;
  case 'r':
    emulator.chip8.key[0xD] = state;
    break;

  // Row 3
  case 'a':
    emulator.chip8.key[0x7] = state;
    break;
  case 's':
    emulator.chip8.key[0x8] = state;
    break;
  case 'd':
    emulator.chip8.key[0x9] = state;
    break;
  case 'f':
    emulator.chip8.key[0xE] = state;
    break;

  // Row 4
  case 'z':
    emulator.chip8.key[0xA] = state;
    break;
  case 'x':
    emulator.chip8.key[0x0] = state;
    break;
  case 'c':
    emulator.chip8.key[0xB] = state;
    break;
  case 'v':
    emulator.chip8.key[0xF] = state;
    break;
  }
}

void drawPixel(int x, int y) {
  const float left = static_cast<float>(x * emulator.window_scale);
  const float right = static_cast<float>((x + 1) * emulator.window_scale);
  const float top = static_cast<float>(y * emulator.window_scale);
  const float bottom = static_cast<float>((y + 1) * emulator.window_scale);

  glBegin(GL_QUADS);
  glVertex2f(left, top);
  glVertex2f(right, top);
  glVertex2f(right, bottom);
  glVertex2f(left, bottom);
  glEnd();
}

void renderScreen() {
  glClear(GL_COLOR_BUFFER_BIT);

  // Render each pixel of the Chip-8 screen
  for (int y = 0; y < CHIP8_SCREEN_HEIGHT; ++y) {
    for (int x = 0; x < CHIP8_SCREEN_WIDTH; ++x) {
      if (emulator.chip8.gfx[y * CHIP8_SCREEN_WIDTH + x] != 0) {
        glColor3f(1.0f, 1.0f, 1.0f); // White for active pixels
        drawPixel(x, y);
      }
    }
  }

  glutSwapBuffers();
}

// GLUT callback functions
void displayCallback() {
  if (emulator.chip8.drawFlag) {
    renderScreen();
    emulator.chip8.drawFlag = false;
  }
}

void idleCallback() {
  emulator.chip8.emulateCycle();

  if (emulator.chip8.drawFlag) {
    glutPostRedisplay();
  }
}

void reshapeCallback(GLsizei width, GLsizei height) {
  if (height == 0)
    height = 1;

  emulator.updateDisplaySize(width, height);

  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, width, height, 0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  std::cout << "Window resized to " << width << "x" << height
            << ", scale: " << emulator.window_scale << std::endl;
}

void keyboardDownCallback(unsigned char key, int x, int y) {
  if (key == 27) { // ESC key
    std::cout << "Exiting..." << std::endl;
    exit(0);
  }
  handleKeyPress(key, true);
}

void keyboardUpCallback(unsigned char key, int x, int y) {
  handleKeyPress(key, false);
}

void initializeGraphics() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black background
  glColor3f(1.0f, 1.0f, 1.0f); // White foreground

  // Set up orthographic projection
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, emulator.display_width, emulator.display_height, 0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void setupGLUT(int argc, char *argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize(emulator.display_width, emulator.display_height);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("Chip-8 Emulator");

  // Register callbacks
  glutDisplayFunc(displayCallback);
  glutIdleFunc(idleCallback);
  glutReshapeFunc(reshapeCallback);
  glutKeyboardFunc(keyboardDownCallback);
  glutKeyboardUpFunc(keyboardUpCallback);

  initializeGraphics();

  std::cout << "Graphics initialized successfully" << std::endl;
  std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
  std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
}

int main(int argc, char *argv[]) {
  // Validate command line arguments
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <rom_file>" << std::endl;
    std::cerr << "Controls:" << std::endl;
    std::cerr << "  1 2 3 4    ->  1 2 3 C" << std::endl;
    std::cerr << "  Q W E R    ->  4 5 6 D" << std::endl;
    std::cerr << "  A S D F    ->  7 8 9 E" << std::endl;
    std::cerr << "  Z X C V    ->  A 0 B F" << std::endl;
    std::cerr << "    ESC      ->   Exit" << std::endl;
    return EXIT_FAILURE;
  }

  // Initialize Chip-8 system
  std::cout << "Initializing Chip-8 system..." << std::endl;
  emulator.chip8.initialize();

  // Load ROM file
  std::cout << "Loading ROM: " << argv[1] << std::endl;
  if (!emulator.chip8.loadGame(argv[1])) {
    std::cerr << "Error: Failed to load ROM file: " << argv[1] << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "ROM loaded successfully!" << std::endl;

  // Setup graphics and start main loop
  setupGLUT(argc, argv);
  std::cout << "Starting emulation... (Press ESC to exit)" << std::endl;
  glutMainLoop();

  return EXIT_SUCCESS;
}

