#ifndef MENU_CONTROLLER_H
#define MENU_CONTROLLER_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>
#include "Shader.h"
#include <glm/common.hpp>
#include <glm/vec3.hpp> 
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include "Input.h"
#include "TextRenderer.h"
#include "MenuItem.h"
#include "Slider.h"
#include "ISlider.h"
#include "Scrollbar.h"
#include "CheckBox.h"
#include "Button.h"

class MenuController
{
private:
	std::vector<MenuItem<float>*> floatItems;
	std::vector<MenuItem<int>*> intItems;
	std::vector<MenuItem<bool>*> boolItems;
	std::vector<MenuItem<std::function<void()>>*> functionItems;
	int numItemsVertically = 0;
	std::vector<float*> floatAdresses;
	std::vector<int*> intAdresses;
	std::vector<bool*> boolAdresses;
	std::vector<std::function<void()>*> functionAdresses;
	float posX, posY, sizeX, sizeY;
	int windowWidth, windowHeight;
	std::vector<float> vert;
	GLuint vao, vbo;
	Shader* baseShader;
	Scrollbar* scrollbar;
		
	bool show_menu;
	bool invisible_menu = false;
	bool holder = false;
	int tempMX, tempMY;
	int tempMXM, tempMYM;
	TextRenderer* txtR;
	float scroll;
	float scrollMax;
	float scrollMin;
	void inintBase();
	void subscribe(MenuItem<float>* item, float* adress);
	void subscribe(MenuItem<int>* item, int* adress);
	void subscribe(MenuItem<bool>* item, bool* adress);
	void subscribe(MenuItem<std::function<void()>>* item, std::function<void()>* adress);
public:
	MenuController(int w, int h);
	~MenuController();
	void clearMenuItems();
	void renderMenu();
	void inputMenu();
	void toggleMenu();
	void toggleInvisible();
	void resize(int w, int h);
	bool getMenuActive() { return show_menu; };
	bool getMenuInvisible() { return invisible_menu; };
	void getMouseXY(int *mx, int *my);
	void setMouseXY(int mx, int my);
	int getItemsNumber();

	void addSlider(std::string name, float* adress, float valueMin, float valueMax, float r, float g, float b, float a);
	void addISlider(std::string name, int* adress, int valueMin, int valueMax, float r, float g, float b, float a);
	void addCheckBox(std::string name, bool* adress, float r, float g, float b, float a);
	void addButton(std::string name, std::function<void()>* adress, float r, float g, float b, float a, bool doBeforeRender = false, bool center = true);
	void addButtons(std::string name, std::vector<std::function<void()>*>* adress, int from, float r, float g, float b, float a, bool doBeforeRender = false);
	void addButtons(std::vector<std::string>* names, std::vector<std::function<void()>*>* adress, float r, float g, float b, float a, bool doBeforeRender = false);
};

#endif // !MENU_CONTROLLER_H