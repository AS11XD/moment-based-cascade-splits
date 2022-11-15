#include "ButtonContainer.h"

ButtonContainer::ButtonContainer(std::function<void(int i)>* func, std::string n, int from, int to, float charSizeX, float x, float y, float sx, float sy, float r, float g, float b, float a)
{
	functions.resize(to - from);
	container.resize(to - from);
	for (int k = 0; k <= to - from; k++) {
		functions[k - from] = new std::function<void()>([&]() { (*func)(k + from); });
		container[k - from] = new Button(functions[k - from], (n + std::to_string(k + from)), charSizeX, x + k * sx / (float)(to - from), y, sx / (float)(to - from), sy, r, g, b, a);
	}
}

ButtonContainer::~ButtonContainer()
{
	for (Button* b : container)
		b->~Button();
	
	container.clear();
	functions.clear();
}

void ButtonContainer::renderItem(TextRenderer* tR, float offsetY, float discardAtYBottom, float discardAtYTop, glm::mat4x4& MVP)
{
	for (Button* b : container)
		b->renderItem(tR, offsetY, discardAtYBottom, discardAtYTop, MVP);
}

void ButtonContainer::input(int w, int h, bool* holder)
{
	for (Button* b : container)
		b->input(w, h, holder);
}
