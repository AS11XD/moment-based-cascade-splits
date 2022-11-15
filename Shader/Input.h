#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <GL\glew.h>
#include <GL/glut.h>
#include <string>

static int  showFPS = 1;
static char windowTitle[80] = "";
static bool keyStates[256];
static bool keySpecialStates[256];
static bool mouseStateLeft = false;
static bool mouseStateMiddle = false;
static bool mouseStateRight = false;
static bool mouseWheelUp = false;
static bool mouseWheelDown = false;
static bool isAltPressed = false;
static bool isCtrlPressed = false;
static bool isShiftPressed = false;
static int  mouseWheelDir = 0;
static int  mouseX = 0;
static int  mouseY = 0;

void mouseMove(int x, int y);

void mouse(int button, int button_state, int x, int y);

void mouseWheel(int button, int dir, int x, int y);

void keyPressed(unsigned char key, int x, int y);

void keyUp(unsigned char key, int x, int y);

void specialKeyPressed(int key, int x, int y);

void specialKeyUp(int key, int x, int y);

void MouseXY(int *x, int *y);

int KeyState(int key);

int SpecialKeyState(int key);

int IsCtrlPressed();

int IsShiftPressed();

int IsAltPressed();

int IsMouseRightPressed();

int IsMouseLeftPressed();

int IsMouseMiddlePressed();

int MouseWheelDir();

#endif //KEYBOARD_H