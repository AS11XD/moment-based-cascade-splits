#include "Slider.h"
#include "MenuController.h"

Slider::Slider(float* ad, std::string n, float textlength, float charSizeX, float minV, float maxV, float x, float y, float sx, float sy, float r, float g, float b, float a) 
	: MenuItem<float>(ad, x, y, sx, sy, r, g, b, a)
{
	sliderShader = new Shader("slider.vert", "slider.frag");
	baseShader = new Shader("sliderBase.vert", "sliderBase.frag");
	min = minV;
	max = maxV;
	name = n;
	csx = charSizeX;
	textlen = textlength;

	vertBase.push_back(x + sx / 2.0f);
	vertBase.push_back(y);
	vertBase.push_back(0.0f);

	vertBase.push_back(0.0f);
	vertBase.push_back(1.0f);

	vertBase.push_back(x + sx / 2.0f);
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

	vertSlider.push_back(x + sx / 2.0f);
	vertSlider.push_back(y);
	vertSlider.push_back(0.0f);

	vertSlider.push_back(0.0f);
	vertSlider.push_back(1.0f);

	vertSlider.push_back(x + sx / 2.0f);
	vertSlider.push_back(y + sy);
	vertSlider.push_back(0.0f);

	vertSlider.push_back(0.0f);
	vertSlider.push_back(0.0f);

	vertSlider.push_back(x + sx / 2.0f + 0.01f);
	vertSlider.push_back(y);
	vertSlider.push_back(0.0f);

	vertSlider.push_back(1.0f);
	vertSlider.push_back(1.0f);

	vertSlider.push_back(x + sx / 2.0f + 0.01f);
	vertSlider.push_back(y + sy);
	vertSlider.push_back(0.0f);

	vertSlider.push_back(1.0f);
	vertSlider.push_back(0.0f);

	glGenVertexArrays(1, &vaoSlider);
	glGenBuffers(1, &vboSlider);

	glBindVertexArray(vaoSlider);

	glBindBuffer(GL_ARRAY_BUFFER, vboSlider);
	glBufferData(GL_ARRAY_BUFFER, vertSlider.size() * sizeof(float), vertSlider.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

Slider::~Slider()
{
	sliderShader->~Shader();
	baseShader->~Shader();
	glDeleteVertexArrays(1, &vaoBase);
	glDeleteBuffers(1, &vboBase);
	vertBase.clear();
	glDeleteVertexArrays(1, &vaoSlider);
	glDeleteBuffers(1, &vboSlider);
	vertSlider.clear();
}

void Slider::renderItem(MenuController* menuController, TextRenderer* tR, float offsetY, float discardAtYBottom, float discardAtYTop, glm::mat4x4& MVP)
{
	if (!menuController->getMenuInvisible()) {
		if (name.compare("") != 0) {
			tR->RenderText(name, posX, posY + offsetY, csx, sizeY, cr, cg, cb, ca, MVP, false, discardAtYBottom, discardAtYTop);
			textlen = tR->getTextSizeX(name, csx);
		}

		baseShader->use();
		baseShader->setFloat("offsetX", 0.0f);
		baseShader->setFloat("offsetY", offsetY);
		baseShader->setFloat("discardAtYBottom", discardAtYBottom);
		baseShader->setFloat("discardAtYTop", discardAtYTop);
		baseShader->setMatrix4x4("MVP", glm::value_ptr(MVP));

		glBindVertexArray(vaoBase);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, vertBase.size() / 5);

		sliderShader->use();
		sliderShader->setFloat("offsetX", 0.0f);
		sliderShader->setFloat("offsetY", offsetY);
		sliderShader->setFloat("discardAtYBottom", discardAtYBottom);
		sliderShader->setFloat("discardAtYTop", discardAtYTop);
		sliderShader->setMatrix4x4("MVP", glm::value_ptr(MVP));
		sliderShader->setFloat("percPos", (*adress - min) / (max - min) * (sizeX / 2.0f - 0.01f));
		
		glBindVertexArray(vaoSlider);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, vertSlider.size() / 5);

		if (name.compare("") != 0) {
			std::ostringstream out;
			out.precision(8);
			out << std::fixed << *adress;
			tR->RenderText(out.str(), posX + sizeX * 0.75f, posY + offsetY + sizeY * 0.1f, csx, sizeY * 0.8f, cr, cg, cb, ca, MVP, true, discardAtYBottom, discardAtYTop);
		}
	}
}

void Slider::input(int w, int h, float offsetY, bool* holder)
{
	int mx, my;
	MouseXY(&mx, &my);
	my = h - my;
	float aspectRat = (float)w / (float)h;
	float mscX = (((float)mx / (float)w) * 2.0f - 1.0f) * aspectRat;
	float mscY = (((float)my / (float)h) * 2.0f - 1.0f);

	if (!(*holder) && !myHold) {
		if (mscX >= posX + textlen && mscX <= posX + sizeX) {
			if (mscY >= posY + offsetY && mscY <= posY + offsetY + sizeY) {
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
		float inne = (mscX - posX - sizeX / 2.0f) / (sizeX / 2.0f - 0.01f);
		inne = inne;
		inne *= (max - min);
		inne += min;
		*adress = std::min(std::max(inne, min), max);
	}
}