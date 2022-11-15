#ifndef SCROLLBAR_H
#define SCROLLBAR_H

#include "MenuItem.h"

class Scrollbar : public MenuItem<float>
{
private:
	Shader* baseShader, * scrollbarShader;
	float *min, *max;
	std::vector<float> vertBase, vertScrollbar;
	GLuint vaoBase, vaoScrollbar, vboBase, vboScrollbar;
	bool myHold = false;
public:
	Scrollbar(float* ad, float* minV, float* maxV, float x, float y, float sx, float sy, float r, float g, float b, float a);
	virtual ~Scrollbar();
	virtual void renderItem(MenuController* menuController, TextRenderer* tR, float offsetY, float discardAtYBottom, float discardAtYTop, glm::mat4x4& MVP);
	virtual void input(int w, int h, float offsetY, bool* holder);
};

#endif // !SCROLLBAR_H