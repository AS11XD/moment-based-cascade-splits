#ifndef SLIDER_H
#define SLIDER_H

#include "MenuItem.h"

class Slider : public MenuItem<float>
{
private:
	Shader* baseShader, *sliderShader;
	float min, max, csx;
	std::vector<float> vertBase, vertSlider;
	GLuint vaoBase, vaoSlider, vboBase, vboSlider;
	std::string name;
	float textlen;
	bool myHold = false;
public:
	Slider(float* ad, std::string n, float textlength, float charSizeX, float minV, float maxV, float x, float y, float sx, float sy, float r, float g, float b, float a);
	virtual ~Slider();
	virtual void renderItem(MenuController* menuController, TextRenderer* tR, float offsetY, float discardAtYBottom, float discardAtYTop, glm::mat4x4& MVP);
	virtual void input(int w, int h, float offsetY, bool* holder);
};

#endif // !SLIDER_H