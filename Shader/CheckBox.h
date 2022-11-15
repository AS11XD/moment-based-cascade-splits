#ifndef CHECKBOX_H
#define CHECKBOX_H

#include "MenuItem.h"

class CheckBox : public MenuItem<bool>
{
private:
	Shader * baseShader, * checkboxShader;
	std::vector<float> vertBase, vertCheckBox;
	GLuint vaoBase, vaoCheckBox, vboBase, vboCheckBox;
	std::string name;
	float textlen, csx;
	bool myHold = false;
public:
	CheckBox(bool* ad, std::string n, float textlength, float charSizeX, float x, float y, float sx, float sy, float r, float g, float b, float a);
	virtual ~CheckBox();
	virtual void renderItem(MenuController* menuController, TextRenderer* tR, float offsetY, float discardAtYBottom, float discardAtYTop, glm::mat4x4& MVP);
	virtual void input(int w, int h, float offsetY, bool* holder);
};

#endif // !CHECKBOX_H