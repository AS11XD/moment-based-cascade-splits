#include "Renderer.h"

Renderer::Renderer(int w, int h, int s, MenuController* mc)
{
	windowWidth = w;
	windowHeight = h;
	seed = seed;
	menuController = mc;
	initFPScounter();
}

Renderer::~Renderer()
{
	deleteFPScounter();
}

void Renderer::startFPSCount()
{
	prevt = std::chrono::high_resolution_clock::now();
}

void Renderer::endFPSCount()
{
	std::chrono::high_resolution_clock::time_point currt = std::chrono::high_resolution_clock::now();
	std::chrono::duration<float> time_span = std::chrono::duration_cast<std::chrono::duration<float>>(currt - prevt);
	float deltaTime = time_span.count();
	if (deltaTime > 0.0f) {
		if (fpsSamples.size() < SAMPLES_NUMBER) {
			fpsSamples.push_back(deltaTime * 1000.0f);
		}
		else {
			fps = calcMedian(fpsSamples);
			fpsSamples.clear();
			fpsSamples.push_back(deltaTime * 1000.0f);
		}
	}
}

void Renderer::initFPScounter()
{
	fpsShader = new Shader("fps.vert", "fps.frag");

	float size = 0.1f;
	int num = 10;
	for (int i = 0; i < num; i++) {
		fpsvert.push_back(i * size - (num - 1) * (size / 2.0f));
		fpsvert.push_back(1.0f - size);
		fpsvert.push_back(0.0f);

		fpsvert.push_back((float)(num - 1) * i / (float) (num - 1));
		fpsvert.push_back(1.0f);

		fpsvert.push_back(i * size - (num - 1) * (size / 2.0f));
		fpsvert.push_back(1.0f);
		fpsvert.push_back(0.0f);

		fpsvert.push_back((float)(num - 1) * i / (float)(num - 1));
		fpsvert.push_back(0.0f);
	}

	glGenVertexArrays(1, &fpsvao);
	glGenBuffers(1, &fpsvbo);

	glBindVertexArray(fpsvao);

	glBindBuffer(GL_ARRAY_BUFFER, fpsvbo);
	glBufferData(GL_ARRAY_BUFFER, fpsvert.size() * sizeof(float), fpsvert.data(), GL_STATIC_DRAW);
	   
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height, nrChannels;
	unsigned char* data = stbi_load("textures/numbers.png", &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "error loading image: numbers\n";
	}
	stbi_image_free(data);
	
	fpsSamples.reserve(SAMPLES_NUMBER);
}

float Renderer::calcMedian(std::vector<float>& m)
{
	size_t size = m.size();
	if (size == 0)
		return 0;
	
	std::sort(m.begin(), m.end());
	if (size % 2 == 0)
		return (m[size / 2 - 1] + m[size / 2]) / 2.0f;
	
	return m[size / 2];
}

void Renderer::deleteFPScounter()
{
	glDeleteVertexArrays(1, &fpsvao);
	glDeleteBuffers(1, &fpsvbo);
	fpsvert.clear();
	fpsShader->~Shader();
	fpsSamples.clear();
}

void Renderer::renderFPScounter()
{
	GLint polygonMod;
	glGetIntegerv(GL_POLYGON_MODE, &polygonMod);

	if (polygonMod != GL_FILL)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex);

	fpsShader->use();
	float aspect = (float)windowWidth / (float)windowHeight;
	glm::mat4 MVP = glm::ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
	fpsShader->setMatrix4x4("MVP", glm::value_ptr(MVP));
	fpsShader->setFloat("fps", fps);
	fpsShader->setInt("tex", 0);

	glBindVertexArray(fpsvao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, fpsvert.size() / 5);
	glEnable(GL_DEPTH_TEST);

	glPolygonMode(GL_FRONT_AND_BACK, polygonMod);
}
