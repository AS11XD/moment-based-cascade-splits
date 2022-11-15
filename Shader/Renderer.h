#ifndef RENDERER_H
#define RENDERER_H
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>
#include "Shader.h"
#include <ctime>
#include <glm/common.hpp>
#include <glm/vec3.hpp> 
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <algorithm>
#include <chrono>
#include "stb_image.h"
#include "MenuController.h"

class Renderer
{
private:
	GLuint fpsvao, fpsvbo, tex;
	std::vector<float> fpsvert;
	Shader* fpsShader;
	std::chrono::high_resolution_clock::time_point prevt;
	float fps;
	const int SAMPLES_NUMBER = 200;
	std::vector<float> fpsSamples;

	void deleteFPScounter();
	void initFPScounter();
	float calcMedian(std::vector<float>& m);
public:
	int windowWidth, windowHeight, seed;
	MenuController* menuController;

	Renderer(int w, int h, int s, MenuController * mc);
	virtual ~Renderer();

	void startFPSCount();
	void endFPSCount();
	void renderFPScounter();

	virtual void render() {};
	virtual void input() {};
	virtual void timer() {};
	virtual void reloadShader() {};
	virtual void resize() {};
	void fetchWH(int w, int h) { windowHeight = h; windowWidth = w; }
};

#endif // !RENDERER_H