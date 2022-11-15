#ifndef ISLIDER_H
#define ISLIDER_H

#include "MenuItem.h"

class ISlider : public MenuItem<int>
{
private:
	Shader* baseShader, * sliderShader;
	float min, max, csx;
	std::vector<float> vertBase, vertSlider;
	GLuint vaoBase, vaoSlider, vboBase, vboSlider;
	std::string name;
	float textlen;
	bool myHold = false;
public:
	ISlider(int* ad, std::string n, float textlength, float charSizeX, int minV, int maxV, float x, float y, float sx, float sy, float r, float g, float b, float a);
	virtual ~ISlider();
	virtual void renderItem(MenuController* menuController, TextRenderer* tR, float offsetY, float discardAtYBottom, float discardAtYTop, glm::mat4x4& MVP);
	virtual void input(int w, int h, float offsetY, bool* holder);
};

#endif // !ISLIDER_H