#ifndef BUTTON_H
#define BUTTON_H

#include "MenuItem.h"
#include <functional>
#include <chrono>
#include <thread>

class Button : public MenuItem<std::function<void()>>
{
private:
	Shader* baseShader;
	std::vector<float> vertBase;
	GLuint vaoBase, vboBase;
	std::string name;
	float csx;
	bool myHold = false;
	bool hover = false;
	bool dbR = false;
	bool dbRAwaiter = false;
	bool dbRMenuToggle = false;
	bool* myHolder = nullptr;
	int myH = 0;
	int myW = 0;

public:
	Button(std::function<void()>* ad, std::string n, float charSizeX, float x, float y, float sx, float sy, float r, float g, float b, float a, bool doBeforeRender = false);
	virtual ~Button();
	virtual void renderItem(MenuController* menuController, TextRenderer* tR, float offsetY, float discardAtYBottom, float discardAtYTop, glm::mat4x4& MVP);
	virtual void input(int w, int h, float offsetY, bool* holder);
};

#endif // !BUTTON_H