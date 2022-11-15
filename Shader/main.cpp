#define _USE_MATH_DEFINES
#include <iostream>
#include <glm/common.hpp>
#include <glm/vec3.hpp> 
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Shader.h"
#include <cmath>
#include "PerlinNoise.h"
#include <ctime>
#include "Input.h"
#include "stb_image.h"
#include "Renderer.h"
#include "CascadeRenderer.h"
#include "MenuController.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

int startW = 1200;
int startH = 800;

Renderer* rend;
MenuController* menuController;
int windowHeight, windowWidth;
std::function<void()> reloadShaderFunc;
std::function<void()> toggleFullscreen;

void timerF()
{
	rend->fetchWH(windowWidth, windowHeight);
	rend->timer();
	menuController->resize(windowWidth, windowHeight);
}

void setVSync(bool sync)
{
	typedef BOOL(APIENTRY* PFNWGLSWAPINTERVALPROC)(int);
	PFNWGLSWAPINTERVALPROC wglSwapIntervalEXT = 0;

	const char* extensions = (char*)glGetString(GL_EXTENSIONS);

	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALPROC)wglGetProcAddress("wglSwapIntervalEXT");

	if (wglSwapIntervalEXT)
		wglSwapIntervalEXT(sync);
}

bool reload = false;
bool swap = false;
bool swap_menu = false;
bool vsnc = false;
bool grid = false;
bool grid_menu = false;
bool swap_grid = false;
bool swapFSC = false;

void initMenuItems()
{
	reloadShaderFunc = [&]() { rend->reloadShader(); };
	menuController->addButton("Reload shader", &reloadShaderFunc, 0.8, 0.8, 0.8, 1.0);
	toggleFullscreen = [&]()
	{
		if (!swapFSC)
			glutFullScreen();
		else
			glutReshapeWindow(startW, startH);
		swapFSC = !swapFSC;
	};
	menuController->addButton("Toggle Fullscreen", &toggleFullscreen, 0.8, 0.8, 0.8, 1.0);
	menuController->addCheckBox("Activate grid view:", &grid, 0.8, 0.8, 0.8, 1.0);
}

void inputF()
{

	if (!reload && KeyState('r'))
	{
		rend->reloadShader();
		reload = true;
	}
	if (reload && !KeyState('r'))
	{
		reload = false;
	}

	if (KeyState('g') && !swap_grid)
	{
		grid = !grid;
		swap_grid = true;
	}
	if (swap_grid && !KeyState('g'))
	{
		swap_grid = false;
	}

	if (grid)
	{
		GLint polygonMod;
		glGetIntegerv(GL_POLYGON_MODE, &polygonMod);

		if (polygonMod == GL_FILL)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		GLint polygonMod;
		glGetIntegerv(GL_POLYGON_MODE, &polygonMod);

		if (polygonMod == GL_LINE)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (!swap_menu && KeyState('e'))
	{
		menuController->toggleMenu();
		swap_menu = true;
	}
	if (swap_menu && !KeyState('e') && !KeyState('t'))
	{
		swap_menu = false;
	}

	if (!swap_menu && KeyState('t'))
	{
		menuController->toggleInvisible();
		swap_menu = true;
	}
	if (swap_menu && !KeyState('t') && !KeyState('e'))
	{
		swap_menu = false;
	}

	if (KeyState(27))
		glutLeaveMainLoop();

	rend->input();
	menuController->inputMenu();
}


//---------------------------------------------------------------------------------------//
bool did = false;
void display(void)
{

	if (!did)
	{
		menuController = new MenuController(windowWidth, windowHeight);
		initMenuItems();

		rend = new CascadeRenderer(windowWidth, windowHeight, 65, menuController);
		did = true;
	}
	else
	{
	}

	timerF();
	inputF();

	rend->render();
}

//---------------------------------------------------------------------------------------//
void resize(int w, int h)
{
	if (!swapFSC)
	{
		startW = w;
		startH = h;
	}

	windowWidth = w;
	windowHeight = h;
	if (rend != nullptr)
	{
		rend->fetchWH(windowWidth, windowHeight);
		rend->resize();
	}
	glViewport(0, 0, w, h);
}

int main(int argc, char** argv)
{
	glutInitContextVersion(4, 5);
	//glutInitContextFlags (GLUT_FORWARD_COMPATIBLE | GLUT_DEBUG);

	glutInit(&argc, argv);
	srand(time(NULL));

	int mode = GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH;
	glutInitDisplayMode(mode);
	glutInitWindowSize(startW, startH);

	glutCreateWindow("Shader");

	glewInit();
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyPressed);
	glutKeyboardUpFunc(keyUp);
	glutSpecialFunc(specialKeyPressed);
	glutSpecialUpFunc(specialKeyUp);
	glutMouseFunc(mouse);
	glutMouseWheelFunc(mouseWheel);
	glutPassiveMotionFunc(mouseMove);
	glutMotionFunc(mouseMove);
	for (int i = 0; i < 256; i++)
	{
		keyStates[i] = false;
		keySpecialStates[i] = false;
	}

	setVSync(vsnc);
	while (ShowCursor(FALSE) >= 0);
	glutMainLoop();

	rend->~Renderer();

	return 0;
}