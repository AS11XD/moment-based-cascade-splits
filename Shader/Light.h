#ifndef LIGHT_H
#define LIGHT_H
#define _USE_MATH_DEFINES

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>
#include "Shader.h"
#include <glm/common.hpp>
#include <glm/vec3.hpp> 
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include "Camera.h"
#include "GLObjModel.h"

class Light
{
private:
	int MAX_CASCADES;
	
	glm::vec3 color;
	float radius;
	bool with_visualized_sm;
	GLuint vao, vbo, ebo;
	std::vector<float> vert;
	std::vector<glm::ivec3> ind;
	std::vector<Camera*> cascades;

	Shader* comp_matVP;
	GLuint cs_matVP, splits;

	glm::vec3 pos, dir, up;

	void genSphere(int slices, int stacks);
public:
	Light(glm::ivec2 nAscpect, glm::vec3 nBoxColor, glm::vec3 nColor, float nRadius, int numberOfCascades, bool nWith_visualized_sm = true);
	~Light();

	void initialize();

	void drawLight(Shader* s);
	
	void drawBox(Shader* s);

	int getMaxCascades() { return MAX_CASCADES; }

	void activateTexturesVisualization(Shader* s, unsigned int fromTexture, int number);
	
	void activateTextures(Shader* s, unsigned int fromTexture, int number);

	void performPrePass(std::vector<std::shared_ptr<GLObjModel>> obj, glm::mat4x4& matM, Shader* s, bool visualize_sm);

	void updateCameraView(glm::vec3 nPos, glm::vec3 nDir, glm::vec3 nUp);

	void updateAspect(int w, int h);

	void updateAspect(glm::ivec2 asp);

	glm::ivec2 getAspect();

	void updateCameraProjection(float left, float right, float bottom, float top, float zNear, float zFar);
	
	void updateCameraProjection(Camera* nc, std::vector<std::vector<GLuint>>* bf, std::vector<std::vector<GLuint>>* bf2, bool bbfit, float overlap);

	float getZNear(int index) { return cascades[index]->getZNear(); }
	float getZFar(int index) { return cascades[index]->getZFar(); }

	std::vector<glm::vec2> getSplits();

	glm::vec3 getDir() { return dir; }
	glm::vec3 getPos() { return pos; }
};

#endif // !LIGHT_H