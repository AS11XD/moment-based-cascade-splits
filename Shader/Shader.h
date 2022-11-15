#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	unsigned int ID;

	Shader(const char* vertexPath, const char* fragmentPath);
	Shader(const char* compPath);
	~Shader();
	
	void use();

	// for compute shaders
	void use(GLuint x, GLuint y, GLuint z);
	void memoryBarrier();

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setIntVec(const std::string& name, const GLsizei size, int* value) const;
	void setFloat(const std::string& name, float value) const;
	void setMatrix4x4(const std::string& name, const GLfloat* value) const;
	void setMatrixArray4x4(const std::string& name, const GLsizei size, const GLfloat* value) const;
	void setMatrix3x3(const std::string& name, const GLfloat* value) const;
	void setFloatArray(const std::string& name, const GLsizei size, const GLfloat* value) const;
	void setVec3(const std::string& name, const GLfloat* value) const;
	void setVec2(const std::string& name, const GLfloat* value) const;
	void setVec2Array(const std::string& name, const GLsizei size, const GLfloat* value) const;
};

#endif // !SHADER_H