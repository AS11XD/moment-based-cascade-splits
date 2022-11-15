#include "MenuController.h"

void MenuController::inintBase()
{
	baseShader = new Shader("menu.vert", "menu.frag");
	scroll = 0.0f;
	scrollMax = 0.0f;
	scrollMin = 0.0f;

	vert.push_back(posX);
	vert.push_back(posY);
	vert.push_back(0.0);

	vert.push_back(0.0);
	vert.push_back(1.0);

	vert.push_back(posX);
	vert.push_back(posY + sizeY);
	vert.push_back(0.0);

	vert.push_back(0.0);
	vert.push_back(0.0);

	vert.push_back(posX + sizeX);
	vert.push_back(posY);
	vert.push_back(0.0);

	vert.push_back(1.0);
	vert.push_back(1.0);

	vert.push_back(posX + sizeX);
	vert.push_back(posY + sizeY);
	vert.push_back(0.0);

	vert.push_back(1.0);
	vert.push_back(0.0);

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), vert.data(), GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	txtR = new TextRenderer();

	scrollbar = new Scrollbar(&scroll,
						   &scrollMin,
						   &scrollMax, 
						   posX + sizeX - 0.03f,
						   posY + 0.01f,
						   0.02f,
						   sizeY - 0.08f,
						   1.0f, 1.0f, 1.0f, 0.5f);
}

MenuController::MenuController(int w, int h)
{
	windowWidth = w;
	windowHeight = h;
	sizeX = 1.6f;
	sizeY = 1.4f;
	posX = -sizeX / 2.0;
	posY = -sizeY / 2.0;
	show_menu = false;
	tempMXM = w / 2;
	tempMYM = h / 2;
	inintBase();
}

MenuController::~MenuController()
{
	clearMenuItems();
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	vert.clear();
	txtR->~TextRenderer();
	free(txtR);
	baseShader->~Shader();
	free(baseShader);
	scrollbar->~Scrollbar();
}

void MenuController::subscribe(MenuItem<float>* item, float* adress)
{
	floatItems.push_back(item);
	floatAdresses.push_back(adress);
}

void MenuController::subscribe(MenuItem<int>* item, int* adress)
{
	intItems.push_back(item);
	intAdresses.push_back(adress);
}

void MenuController::subscribe(MenuItem<bool>* item, bool* adress)
{	
	boolItems.push_back(item);
	boolAdresses.push_back(adress);
}

void MenuController::subscribe(MenuItem<std::function<void()>>* item, std::function<void()>* adress)
{
	functionItems.push_back(item);
	functionAdresses.push_back(adress);
}

void MenuController::clearMenuItems()
{
	for (MenuItem<float> *i : floatItems) {
		i->~MenuItem();
	}
	for (MenuItem<int> *i : intItems) {
		i->~MenuItem();
	}
	for (MenuItem<bool>* i : boolItems) {
		i->~MenuItem();
	}
	for (MenuItem<std::function<void()>>* i : functionItems) {
		i->~MenuItem();
	}

	numItemsVertically = 0;
	floatItems.clear();
	intItems.clear();
	boolItems.clear();
	floatAdresses.clear();
	intAdresses.clear();
	boolAdresses.clear();
	functionAdresses.clear();
	functionItems.clear();
	scroll = 0.0f;
	scrollMax = 0.0f;
	scrollMin = 0.0f;
}

void MenuController::resize(int w, int h)
{
	windowWidth = w;
	windowHeight = h;
}

void MenuController::getMouseXY(int* mx, int* my)
{
	*mx = tempMX;
	*my = tempMY;
}

void MenuController::setMouseXY(int mx, int my)
{
	tempMX = mx;
	tempMY = my;
}

int MenuController::getItemsNumber()
{
	return floatItems.size() + boolItems.size() + functionItems.size() + intItems.size() - numItemsVertically;
}

void MenuController::addSlider(std::string name, float* adress, float valueMin, float valueMax, float r, float g, float b, float a)
{
	for (float* ad : floatAdresses) {
		if (ad == adress)
			return;
	}

	float scr = sizeY - 0.07f * (getItemsNumber() + 2) - 0.01f;
	if (scr < 0.0f)
		scrollMax = -scr;
	else
		scrollMax = 0.0f;

	Slider* slider = new Slider(adress,
								name,
								posX + 0.01f,
								0.05f, 
								valueMin, 
								valueMax, 
								posX + 0.01f, 
								posY + sizeY - 0.07f * (getItemsNumber() + 2),
								sizeX - 0.05f, 
								0.05f,
								r, g, b, a);

	subscribe(slider, adress);
}

void MenuController::addISlider(std::string name, int* adress, int valueMin, int valueMax, float r, float g, float b, float a)
{
	for (int* ad : intAdresses) {
		if (ad == adress)
			return;
	}

	float scr = sizeY - 0.07f * (getItemsNumber() + 2) - 0.01f;
	if (scr < 0.0f)
		scrollMax = -scr;
	else
		scrollMax = 0.0f;

	ISlider* slider = new ISlider(adress,
		name,
		posX + 0.01f,
		0.05f,
		valueMin,
		valueMax,
		posX + 0.01f,
		posY + sizeY - 0.07f * (getItemsNumber() + 2),
		sizeX - 0.05f,
		0.05f,
		r, g, b, a);

	subscribe(slider, adress);
}

void MenuController::addCheckBox(std::string name, bool* adress, float r, float g, float b, float a)
{
	for (bool* ad : boolAdresses) {
		if (ad == adress)
			return;
	}

	float scr = sizeY - 0.07f * (getItemsNumber() + 2) - 0.01f;
	if (scr < 0.0f)
		scrollMax = -scr;
	else
		scrollMax = 0.0f;

	CheckBox* checkBox = new CheckBox(adress,
		name,
		txtR->getTextSizeX(name, 0.05f),
		0.05f,
		posX + 0.01f,
		posY + sizeY - 0.07f * (getItemsNumber() + 2),
		sizeX - 0.05f,
		0.05f,
		r, g, b, a);

	subscribe(checkBox, adress);
}

void MenuController::addButton(std::string name, std::function<void()>* adress, float r, float g, float b, float a, bool doBeforeRender, bool center)
{
	for (std::function<void()>* ad : functionAdresses) {
		if (ad == adress)
			return;
	}

	float scr = sizeY - 0.07f * (getItemsNumber() + 2) - 0.01f;
	if (scr < 0.0f)
		scrollMax = -scr;
	else
		scrollMax = 0.0f;

	float offs = center ? (sizeX - 0.05f - txtR->getTextSizeX(name, 0.05f) * 1.1f) / 2.0f : 0.0f;

	Button* button = new Button(adress,
		name,
		0.05f,
		posX + 0.01f + offs,
		posY + sizeY - 0.07f * (getItemsNumber() + 2),
		txtR->getTextSizeX(name, 0.05f) * 1.1f,
		0.05f,
		r, g, b, a, doBeforeRender);

	subscribe(button, adress);
}

void MenuController::addButtons(std::string name, std::vector<std::function<void()>*>* adress, int from, float r, float g, float b, float a, bool doBeforeRender)
{
	for (int k = 0; k < adress->size(); k++) {
		for (std::function<void()>* ad : functionAdresses) {
			if (ad == adress->at(k))
				return;
		}
	}

	float scr = sizeY - 0.07f * (getItemsNumber() + 2) - 0.01f;
	if (scr < 0.0f)
		scrollMax = -scr;
	else
		scrollMax = 0.0f;

	for (int k = 0; k < adress->size(); k++) {
		std::string nName = (name + std::to_string(k + from));
		Button* button = new Button(adress->at(k),
			nName,
			0.05f,
			posX + 0.01f + k * ((sizeX - 0.05f) / (float)(adress->size())) + (sizeX - 0.05f) / (float)(adress->size()) * 0.1f,
			posY + sizeY - 0.07f * (getItemsNumber() + 2),
			(sizeX - 0.05f) / (float)(adress->size()) * 0.8f,
			0.05f,
			r, g, b, a, doBeforeRender);

		numItemsVertically++;
		subscribe(button, adress->at(k));
	}
	numItemsVertically--;
}

void MenuController::addButtons(std::vector<std::string>* names, std::vector<std::function<void()>*>* adress, float r, float g, float b, float a, bool doBeforeRender)
{
	if (names->size() != adress->size()) {
		std::cout << "error, names must be as large as adresses\n";
		return;
	}

	for (int k = 0; k < adress->size(); k++) {
		for (std::function<void()>* ad : functionAdresses) {
			if (ad == adress->at(k))
				return;
		}
	}

	float scr = sizeY - 0.07f * (getItemsNumber() + 2) - 0.01f;
	if (scr < 0.0f)
		scrollMax = -scr;
	else
		scrollMax = 0.0f;

	for (int k = 0; k < adress->size(); k++) {
		Button* button = new Button(adress->at(k),
			names->at(k),
			0.05f,
			posX + 0.01f + k * ((sizeX - 0.05f) / (float)(adress->size())) + (sizeX - 0.05f) / (float)(adress->size()) * 0.1f,
			posY + sizeY - 0.07f * (getItemsNumber() + 2),
			(sizeX - 0.05f) / (float)(adress->size()) * 0.8f,
			0.05f,
			r, g, b, a, doBeforeRender);

		numItemsVertically++;
		subscribe(button, adress->at(k));
	}
	numItemsVertically--;
}

void MenuController::renderMenu()
{
	if (show_menu) {
		if (!invisible_menu) {
			GLint polygonMod;
			glm::mat4 MVP;
			glGetIntegerv(GL_POLYGON_MODE, &polygonMod);

			if (polygonMod != GL_FILL)
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			glEnable(GL_BLEND);
			glBlendEquation(GL_FUNC_ADD);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			baseShader->use();
			float aspect = (float)windowWidth / (float)windowHeight;
			MVP = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
			baseShader->setMatrix4x4("MVP", glm::value_ptr(MVP));
			glBindVertexArray(vao);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, vert.size() / 5);

			txtR->RenderText("MENU", 0.0, posY + sizeY - 0.07, 0.06, 0.06, 0.0, 1.0, 1.0, 0.8, MVP, true);
		
			for (MenuItem<float>* item : floatItems) {
				item->renderItem(this, txtR, scroll, posY + 0.01f, posY + sizeY - 0.07f, MVP);
			}
			
			for (MenuItem<int>* item : intItems) {
				item->renderItem(this, txtR, scroll, posY + 0.01f, posY + sizeY - 0.07f, MVP);
			}

			for (MenuItem<bool>* item : boolItems) {
				item->renderItem(this, txtR, scroll, posY + 0.01f, posY + sizeY - 0.07f, MVP);
			}

			for (MenuItem<std::function<void()>>* item : functionItems) {
				item->renderItem(this, txtR, scroll, posY + 0.01f, posY + sizeY - 0.07f, MVP);
			}

			if (scrollMax > 0.0f)
				scrollbar->renderItem(this, txtR, 0.0f, -100.0f, 100.0f, MVP);

			glDisable(GL_BLEND);
			glDepthMask(GL_TRUE);
			glEnable(GL_DEPTH_TEST);
			glPolygonMode(GL_FRONT_AND_BACK, polygonMod);
		}
		else {
			glm::mat4 MVP = glm::mat4x4(1.0f);
			for (MenuItem<float>* item : floatItems) {
				item->renderItem(this, txtR, scroll, posY + 0.01f, posY + sizeY - 0.07f, MVP);
			}
			
			for (MenuItem<int>* item : intItems) {
				item->renderItem(this, txtR, scroll, posY + 0.01f, posY + sizeY - 0.07f, MVP);
			}

			for (MenuItem<bool>* item : boolItems) {
				item->renderItem(this, txtR, scroll, posY + 0.01f, posY + sizeY - 0.07f, MVP);
			}

			for (MenuItem<std::function<void()>>* item : functionItems) {
				item->renderItem(this, txtR, scroll, posY + 0.01f, posY + sizeY - 0.07f, MVP);
			}

			if (scrollMax > 0.0f)
				scrollbar->renderItem(this, txtR, 0.0f, -100.0f, 100.0f, MVP);
		}
	}
}

void MenuController::inputMenu()
{
	if (show_menu) {
		for (MenuItem<float>* item : floatItems) {
			item->input(windowWidth, windowHeight, scroll, &holder);
		}
		
		for (MenuItem<int>* item : intItems) {
			item->input(windowWidth, windowHeight, scroll, &holder);
		}

		for (MenuItem<bool>* item : boolItems) {
			item->input(windowWidth, windowHeight, scroll, &holder);
		}

		for (MenuItem<std::function<void()>>* item : functionItems) {
			item->input(windowWidth, windowHeight, scroll, &holder);
		}
		
		if (scrollMax > 0.0f)
			scrollbar->input(windowWidth, windowHeight, 0.0f, &holder);
	}
}

void MenuController::toggleMenu()
{
	show_menu = !show_menu;
	if (show_menu) {
		while (ShowCursor(TRUE) < 0);
		MouseXY(&tempMX, &tempMY);
		glutWarpPointer(tempMXM, tempMYM);
	}
	else {
		while (ShowCursor(FALSE) >= 0);
		MouseXY(&tempMXM, &tempMYM);
		glutWarpPointer(tempMX, tempMY);
	}
}

void MenuController::toggleInvisible()
{
	invisible_menu = !invisible_menu;
}