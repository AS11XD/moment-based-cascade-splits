#include "Scrollbar.h"
#include "MenuController.h"

Scrollbar::Scrollbar(float* ad, float* minV, float* maxV, float x, float y, float sx, float sy, float r, float g, float b, float a)
	: MenuItem<float>(ad, x, y, sx, sy, r, g, b, a)
{
	scrollbarShader = new Shader("scrollbar.vert", "scrollbar.frag");
	baseShader = new Shader("scrollbarBase.vert", "scrollbarBase.frag");
	min = minV;
	max = maxV;

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

	glGenVertexArrays(1, &vaoScrollbar);
	glGenBuffers(1, &vboScrollbar);
}

Scrollbar::~Scrollbar()
{
	scrollbarShader->~Shader();
	baseShader->~Shader();
	glDeleteVertexArrays(1, &vaoBase);
	glDeleteBuffers(1, &vboBase);
	vertBase.clear();
	glDeleteVertexArrays(1, &vaoScrollbar);
	glDeleteBuffers(1, &vboScrollbar);
	vertScrollbar.clear();
}

void Scrollbar::renderItem(MenuController* menuController, TextRenderer* tR, float offsetY, float discardAtYBottom, float discardAtYTop, glm::mat4x4& MVP)
{
	if (!menuController->getMenuInvisible()) {

		vertScrollbar.push_back(posX);
		vertScrollbar.push_back(posY + sizeY);
		vertScrollbar.push_back(0.0f);
		vertScrollbar.push_back(0.0f);
		vertScrollbar.push_back(1.0f);

		vertScrollbar.push_back(posX + sizeX);
		vertScrollbar.push_back(posY + sizeY);
		vertScrollbar.push_back(0.0f);
		vertScrollbar.push_back(0.0f);
		vertScrollbar.push_back(0.0f);

		vertScrollbar.push_back(posX);
		vertScrollbar.push_back(posY + sizeY - sizeY * sizeY / (*max - *min + sizeY));
		vertScrollbar.push_back(0.0f);
		vertScrollbar.push_back(1.0f);
		vertScrollbar.push_back(1.0f);

		vertScrollbar.push_back(posX + sizeX);
		vertScrollbar.push_back(posY + sizeY - sizeY * sizeY / (*max - *min + sizeY));
		vertScrollbar.push_back(0.0f);
		vertScrollbar.push_back(1.0f);
		vertScrollbar.push_back(0.0f);

		glBindVertexArray(vaoScrollbar);
		glBindBuffer(GL_ARRAY_BUFFER, vboScrollbar);
		glBufferData(GL_ARRAY_BUFFER, vertScrollbar.size() * sizeof(float), vertScrollbar.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		baseShader->use();
		baseShader->setFloat("offsetY", offsetY);
		baseShader->setFloat("discardAtYBottom", discardAtYBottom);
		baseShader->setFloat("discardAtYTop", discardAtYTop);
		baseShader->setMatrix4x4("MVP", glm::value_ptr(MVP));

		glBindVertexArray(vaoBase);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, vertBase.size() / 5);

		scrollbarShader->use();
		scrollbarShader->setFloat("offsetY", offsetY);
		scrollbarShader->setFloat("discardAtYBottom", discardAtYBottom);
		scrollbarShader->setFloat("discardAtYTop", discardAtYTop);
		scrollbarShader->setMatrix4x4("MVP", glm::value_ptr(MVP));
		scrollbarShader->setFloat("percPos", -(*adress - *min) / (*max - *min) * (sizeY - sizeY * sizeY / (*max - *min + sizeY)));

		glBindVertexArray(vaoScrollbar);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, vertScrollbar.size() / 5);
	}
}

void Scrollbar::input(int w, int h, float offsetY, bool* holder)
{
	int mx, my;
	MouseXY(&mx, &my);
	my = h - my;
	float aspectRat = (float)w / (float)h;
	float mscX = (((float)mx / (float)w) * 2.0f - 1.0f) * aspectRat;
	float mscY = (((float)my / (float)h) * 2.0f - 1.0f);

	if (!(*holder) && !myHold) {
		if (mscX >= posX && mscX <= posX + sizeX) {
			if (mscY >= posY + offsetY && mscY <= posY + sizeY + offsetY) {
				if (IsMouseLeftPressed()) {
					*holder = true;
					myHold = true;
				}
			}
		}
	}
	else if (!IsMouseLeftPressed()) {
		*holder = false;
		myHold = false;
	}

	if (*holder && myHold) {
		float inne = (mscY + offsetY - posY) / sizeY;
		inne -= sizeY * sizeY / (*max - *min + sizeY) * 0.5f;
		inne /= sizeY - sizeY * sizeY / (*max - *min + sizeY);
		inne = (1.0f - inne);
		inne *= (*max - *min);
		inne += *min;
		*adress = std::min(std::max(inne, *min), *max);
	}

	int dir = MouseWheelDir();
	if (dir > 0) {
		*adress = std::min(std::max(*adress - 0.04f, *min), *max);
	}
	if (dir < 0) {
		*adress = std::min(std::max(*adress + 0.04f, *min), *max);
	}
}