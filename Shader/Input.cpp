#include "Input.h"

void mouseMove(int x, int y) {
	mouseX = x;
	mouseY = y;
}

void mouse(int button, int button_state, int x, int y)
{
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		mouseStateLeft = (button_state == GLUT_DOWN) ? true : false;
		break;
	case GLUT_MIDDLE_BUTTON:
		mouseStateMiddle = (button_state == GLUT_DOWN) ? true : false;
		break;
	case GLUT_RIGHT_BUTTON:
		mouseStateRight = (button_state == GLUT_DOWN) ? true : false;
		break;
	case 3:
		mouseWheelUp = true;
		break;
	case 4:
		mouseWheelDown = true;
		break;
	}
}

void mouseWheel(int button, int dir, int x, int y) {
	mouseWheelDir = dir;
}


void keyPressed(unsigned char key, int x, int y) {
	isCtrlPressed = glutGetModifiers() == GLUT_ACTIVE_CTRL;
	isShiftPressed = glutGetModifiers() == GLUT_ACTIVE_SHIFT;
	isAltPressed = glutGetModifiers() == GLUT_ACTIVE_ALT;
	keyStates[key] = true;
	if (key >= 'a' && key <= 'z') {
		keyStates[key - 'a' + 'A'] = true;
	}
	if (key >= 'A' && key <= 'Z') {
		keyStates[key - 'A' + 'a'] = true;
	}

	glutPostRedisplay();
}


void keyUp(unsigned char key, int x, int y) {
	isCtrlPressed = glutGetModifiers() == GLUT_ACTIVE_CTRL;
	isShiftPressed = glutGetModifiers() == GLUT_ACTIVE_SHIFT;
	isAltPressed = glutGetModifiers() == GLUT_ACTIVE_ALT;
	keyStates[key] = false;
	if (key >= 'a' && key <= 'z') {
		keyStates[key - 'a' + 'A'] = false;
	}
	if (key >= 'A' && key <= 'Z') {
		keyStates[key - 'A' + 'a'] = false;
	}
}

void specialKeyPressed(int key, int x, int y) {
	isCtrlPressed = glutGetModifiers() == GLUT_ACTIVE_CTRL;
	isShiftPressed = glutGetModifiers() == GLUT_ACTIVE_SHIFT;
	isAltPressed = glutGetModifiers() == GLUT_ACTIVE_ALT;
	keySpecialStates[key] = true;
}

void specialKeyUp(int key, int x, int y) {
	isCtrlPressed = glutGetModifiers() == GLUT_ACTIVE_CTRL;
	isShiftPressed = glutGetModifiers() == GLUT_ACTIVE_SHIFT;
	isAltPressed = glutGetModifiers() == GLUT_ACTIVE_ALT;
	keySpecialStates[key] = false;
}

void MouseXY(int *x, int *y) {
	*x = mouseX;
	*y = mouseY;
}

int KeyState(int key) {
	if (key > 0 && key < 256) {
		return keyStates[key];
	}
	return 0;
}

int SpecialKeyState(int key) {
	if (key > 0 && key < 248) {
		return keySpecialStates[key];
	}
	return 0;
}



int IsCtrlPressed() {
	return isCtrlPressed;
}

int IsShiftPressed() {
	return isShiftPressed;
}

int IsAltPressed() {
	return isAltPressed;
}

int IsMouseRightPressed() {
	return mouseStateRight;
}

int IsMouseLeftPressed() {
	return mouseStateLeft;
}

int IsMouseMiddlePressed() {
	return mouseStateMiddle;
}

int MouseWheelDir() {
	int h = mouseWheelDir;
	mouseWheelDir = 0;
	return h;
}