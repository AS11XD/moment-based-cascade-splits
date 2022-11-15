#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>
#include "Shader.h"
#include <algorithm>
#include "stb_image.h"
#include <glm/common.hpp>
#include <glm/vec3.hpp> 
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp> 
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

class TextRenderer
{
private:
	GLuint vao, vbo, tex;
	Shader* s;
public:
	TextRenderer();
	~TextRenderer();
	
	float getTextSizeX(std::string text, float sizeX);
	void RenderText(std::string text, float x, float y, float sizeX, float sizeY, float r, float g, float b, float a, glm::mat4x4& MVP, bool center, float discardAtYBottom = -1000.0f, float discardAtYTop = 1000.0f);
};

#endif // !TEXT_RENDERER_H