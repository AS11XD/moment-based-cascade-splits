#include "Button.h"
#include "MenuController.h"

Button::Button(std::function<void()>* ad, std::string n, float charSizeX, float x, float y, float sx, float sy, float r, float g, float b, float a, bool doBeforeRender)
	: MenuItem<std::function<void()>>(ad, x, y, sx, sy, r, g, b, a)
{
	baseShader = new Shader("button.vert", "button.frag");
	name = n;
	csx = charSizeX;
	dbR = doBeforeRender;

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

	vertBase.push_back(x + sx);
	vertBase.push_back(y);
	vertBase.push_back(0.0f);

	vertBase.push_back(1.0f);
	vertBase.push_back(1.0f);

	vertBase.push_back(x + sx);
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
}

Button::~Button()
{
	baseShader->~Shader();
	glDeleteVertexArrays(1, &vaoBase);
	glDeleteBuffers(1, &vboBase);
	vertBase.clear();
}

void Button::renderItem(MenuController* menuController, TextRenderer* tR, float offsetY, float discardAtYBottom, float discardAtYTop, glm::mat4x4& MVP)
{
	if (dbR) {
		int mx, my;
		MouseXY(&mx, &my);
		my = myH - my;
		float aspectRat = (float)myW / (float)myH;
		float mscX = (((float)mx / (float)myW) * 2.0f - 1.0f) * aspectRat;
		float mscY = (((float)my / (float)myH) * 2.0f - 1.0f);

		hover = ((mscX >= posX && mscX <= posX + sizeX) && (mscY >= posY + offsetY && mscY <= posY + sizeY + offsetY));

		if (dbRAwaiter) {
			(*adress)();
			dbRAwaiter = false;
			if (dbRMenuToggle) {
				menuController->toggleInvisible();
				dbRMenuToggle = false;
			}
		}

		if (!(*myHolder) && !myHold) {
			if (hover) {
				if (IsMouseLeftPressed()) {
					*myHolder = true;
					myHold = true;
				}
			}
		}
		else if (!IsMouseLeftPressed()) {
			*myHolder = false;
			if (hover) {
				dbRAwaiter = true;
				dbRMenuToggle = (!menuController->getMenuInvisible());
				if (dbRMenuToggle) {
					menuController->toggleInvisible();
					myHold = false;
					return;
				}
			}
			myHold = false;
		}
	}

	if (!menuController->getMenuInvisible()) {
		baseShader->use();
		baseShader->setFloat("offsetY", offsetY);
		baseShader->setFloat("discardAtYBottom", discardAtYBottom);
		baseShader->setFloat("discardAtYTop", discardAtYTop);
		baseShader->setBool("hover", hover);
		baseShader->setBool("myHold", myHold);
		baseShader->setMatrix4x4("MVP", glm::value_ptr(MVP));

		glBindVertexArray(vaoBase);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, vertBase.size() / 5);

		if (name.compare("") != 0)
			tR->RenderText(name, posX + sizeX / 2.0, posY + offsetY, csx, sizeY, cr, cg, cb, ca, MVP, true, discardAtYBottom, discardAtYTop);
	}
	
}

void Button::input(int w, int h, float offsetY, bool* holder)
{
	int mx, my;
	MouseXY(&mx, &my);
	my = h - my;
	float aspectRat = (float)w / (float)h;
	float mscX = (((float)mx / (float)w) * 2.0f - 1.0f) * aspectRat;
	float mscY = (((float)my / (float)h) * 2.0f - 1.0f);
	   
	if (!dbR) {
		hover = ((mscX >= posX && mscX <= posX + sizeX) && (mscY >= posY + offsetY && mscY <= posY + sizeY + offsetY));

		if (!(*holder) && !myHold) {
			if (hover) {
				if (IsMouseLeftPressed()) {
					*holder = true;
					myHold = true;
				}
			}
		}
		else if (!IsMouseLeftPressed()) {
			*holder = false;
			if (hover) {
				(*adress)();
			}
			myHold = false;
		}
	}
	else {
		myHolder = holder;
		myW = w;
		myH = h;
	}
}