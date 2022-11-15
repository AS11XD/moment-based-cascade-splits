#ifndef MENU_ITEM_H
#define MENU_ITEM_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>
#include "Shader.h"
#include "Input.h"
#include "TextRenderer.h"
#include <glm/common.hpp>
#include <glm/vec3.hpp> 
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

class MenuController;

template <typename T> class MenuItem
{
protected:
	float posX, posY, sizeX, sizeY;
	float cr, cg, cb, ca;
	T* adress;
	
public:
	MenuItem(T* ad, float x, float y, float sx, float sy, float r, float g, float b, float a);
	~MenuItem();
	virtual void renderItem(MenuController* menuController, TextRenderer* tR, float offsetY, float discardAtYBottom, float discardAtYTop, glm::mat4x4& MVP);
	virtual void input(int w, int h, float offsetY, bool* holder);
};

template<typename T>
MenuItem<T>::MenuItem(T* ad, float x, float y, float sx, float sy, float r, float g, float b, float a)
{
	adress = ad;
	posX = x;
	posY = y;
	sizeX = sx;
	sizeY = sy;
	cr = r;
	cg = g;
	cb = b;
	ca = a;
}

template<typename T>
MenuItem<T>::~MenuItem()
{
}

template<typename T>
inline void MenuItem<T>::renderItem(MenuController* menuController, TextRenderer* tR, float offsetY, float discardAtYBottom, float discardAtYTop, glm::mat4x4& MVP)
{
}


template<typename T>
void MenuItem<T>::input(int w, int h, float offsetY, bool* holder)
{
}

#endif // !MENU_ITEM_H