#include "CheckBox.h"
#include "MenuController.h"

CheckBox::CheckBox(bool* ad, std::string n, float textlength, float charSizeX, float x, float y, float sx, float sy, float r, float g, float b, float a)
	: MenuItem<bool>(ad, x, y, sx, sy, r, g, b, a)
{
	checkboxShader = new Shader("checkbox.vert", "checkbox.frag");
	baseShader = new Shader("checkboxBase.vert", "checkboxBase.frag");
	name = n;
	textlen = textlength;
	csx = charSizeX;

	vertBase.push_back(x);
	vertBase.push_back(y);
	vertBase.push_back(0.0f);

	vertBase.push_back(0.0f);
	vertBase.push_back(1.0f);

	vertBase.push_back(x);
	vertBase.push_back(y + sy);
	vertBase.push_back(0.0f);

	vertBase.push_back(0.0f);
	vertBase.push_back(0.0f);

	vertBase.push_back(x + sy);
	vertBase.push_back(y);
	vertBase.push_back(0.0f);

	vertBase.push_back(1.0f);
	vertBase.push_back(1.0f);

	vertBase.push_back(x + sy);
	vertBase.push_back(y + sy);
	vertBase.push_back(0.0f);

	vertBase.push_back(1.0f);
	vertBase.push_back(0.0f);

	glGenVertexArrays(1, &vaoBase);
	glGenBuffers(1, &vboBase);

	glBindVertexArray(vaoBase);

	glBindBuffer(GL_ARRAY_BUFFER, vboBase);
	glBufferData(GL_ARRAY_BUFFER, vertBase.size() * sizeof(float), vertBase.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	vertCheckBox.push_back(x + sy * 0.15f);
	vertCheckBox.push_back(y + sy * 0.15f);
	vertCheckBox.push_back(0.0f);

	vertCheckBox.push_back(0.0f);
	vertCheckBox.push_back(1.0f);

	vertCheckBox.push_back(x + sy * 0.15f);
	vertCheckBox.push_back(y + sy * 0.85f);
	vertCheckBox.push_back(0.0f);

	vertCheckBox.push_back(0.0f);
	vertCheckBox.push_back(0.0f);

	vertCheckBox.push_back(x + sy * 0.85f);
	vertCheckBox.push_back(y + sy * 0.15f);
	vertCheckBox.push_back(0.0f);

	vertCheckBox.push_back(1.0f);
	vertCheckBox.push_back(1.0f);

	vertCheckBox.push_back(x + sy * 0.85f);
	vertCheckBox.push_back(y + sy * 0.85f);
	vertCheckBox.push_back(0.0f);

	vertCheckBox.push_back(1.0f);
	vertCheckBox.push_back(0.0f);

	glGenVertexArrays(1, &vaoCheckBox);
	glGenBuffers(1, &vboCheckBox);

	glBindVertexArray(vaoCheckBox);

	glBindBuffer(GL_ARRAY_BUFFER, vboCheckBox);
	glBufferData(GL_ARRAY_BUFFER, vertCheckBox.size() * sizeof(float), vertCheckBox.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

CheckBox::~CheckBox()
{
	checkboxShader->~Shader();
	baseShader->~Shader();
	glDeleteVertexArrays(1, &vaoBase);
	glDeleteBuffers(1, &vboBase);
	vertBase.clear();
	glDeleteVertexArrays(1, &vaoCheckBox);
	glDeleteBuffers(1, &vboCheckBox);
	vertCheckBox.clear();
}

void CheckBox::renderItem(MenuController* menuController, TextRenderer* tR, float offsetY, float discardAtYBottom, float discardAtYTop, glm::mat4x4& MVP)
{
	if (!menuController->getMenuInvisible()) {
		if (name.compare("") != 0) {
			tR->RenderText(name, posX, posY + offsetY, csx, sizeY, cr, cg, cb, ca, MVP, false, discardAtYBottom, discardAtYTop);
			textlen = tR->getTextSizeX(name, csx);
		}

		baseShader->use();
		baseShader->setFloat("offsetX", sizeX - sizeY * 1.1);
		baseShader->setFloat("offsetY", offsetY);
		baseShader->setFloat("discardAtYBottom", discardAtYBottom);
		baseShader->setFloat("discardAtYTop", discardAtYTop);
		baseShader->setMatrix4x4("MVP", glm::value_ptr(MVP));

		glBindVertexArray(vaoBase);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, vertBase.size() / 5);

		checkboxShader->use();
		checkboxShader->setFloat("offsetX", sizeX - sizeY * 1.1);
		checkboxShader->setFloat("offsetY", offsetY);
		checkboxShader->setFloat("discardAtYBottom", discardAtYBottom);
		checkboxShader->setFloat("discardAtYTop", discardAtYTop);
		checkboxShader->setMatrix4x4("MVP", glm::value_ptr(MVP));
		checkboxShader->setBool("activeBox", *adress);

		glBindVertexArray(vaoCheckBox);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, vertCheckBox.size() / 5);
	}
}

void CheckBox::input(int w, int h, float offsetY, bool* holder)
{
	int mx, my;
	MouseXY(&mx, &my);
	my = h - my;
	float aspectRat = (float)w / (float)h;
	float mscX = (((float)mx / (float)w) * 2.0f - 1.0f) * aspectRat;
	float mscY = (((float)my / (float)h) * 2.0f - 1.0f);

	if (!(*holder) && !myHold) {
		if ((mscX >= posX + sizeX - sizeY * 1.1 && mscX <= posX + sizeX - sizeY * 1.1 + sizeY) && (mscY >= posY + offsetY && mscY <= posY + sizeY + offsetY)) {
			if (IsMouseLeftPressed()) {
				*holder = true;
				myHold = true;
			}
		}
	}
	else if (!IsMouseLeftPressed()) {
		*holder = false;
		if (myHold && (mscX >= posX + sizeX - sizeY * 1.1 && mscX <= posX + sizeX - sizeY * 1.1 + sizeY + offsetY) && (mscY >= posY && mscY <= posY + sizeY + offsetY)) {
			*adress = !(*adress);
		}
		myHold = false;
	}
}