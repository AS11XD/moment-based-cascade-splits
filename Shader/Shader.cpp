#include "Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		vShaderFile.close();
		fShaderFile.close();
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "error while reading files" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "vertex shader compilation failed\n" << infoLog << std::endl;
	};
	
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "fragment shader compilation failed\n" << infoLog << std::endl;
	};

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "shader linking failed\n" << infoLog << std::endl;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader::Shader(const char* compPath)
{
	std::string compCode;
	std::ifstream cShaderFile;
	cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		cShaderFile.open(compPath);
		std::stringstream cShaderStream;
		cShaderStream << cShaderFile.rdbuf();
		cShaderFile.close();
		compCode = cShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "error while reading files" << std::endl;
	}
	const char* cShaderCode = compCode.c_str();
	
	unsigned int computation;
	int success;
	char infoLog[512];

	computation = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(computation, 1, &cShaderCode, NULL);
	glCompileShader(computation);
	glGetShaderiv(computation, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(computation, 512, NULL, infoLog);
		std::cout << "computation shader compilation failed\n" << infoLog << std::endl;
	};

	ID = glCreateProgram();
	glAttachShader(ID, computation);
	glLinkProgram(ID);
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "shader linking failed\n" << infoLog << std::endl;
	}

	glDeleteShader(computation);
}

Shader::~Shader()
{
	glDeleteProgram(ID);
}

void Shader::use()
{
	glUseProgram(ID);
}

void Shader::use(GLuint x, GLuint y, GLuint z)
{
	glUseProgram(ID);
	glDispatchCompute(x, y, z);
}

void Shader::memoryBarrier()
{
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}

void Shader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setIntVec(const std::string& name, const GLsizei size, int* value) const
{
	glUniform1iv(glGetUniformLocation(ID, name.c_str()), size, value);
}
void Shader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setMatrix4x4(const std::string& name, const GLfloat* value) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE ,value);
}
void Shader::setMatrixArray4x4(const std::string& name, const GLsizei size, const GLfloat* value) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), size, GL_FALSE, value);
}
void Shader::setMatrix3x3(const std::string& name, const GLfloat* value) const
{
	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, value);
}
void Shader::setFloatArray(const std::string& name, const GLsizei size, const GLfloat* value) const
{
	glUniform1fv(glGetUniformLocation(ID, name.c_str()), size, value);
}

void Shader::setVec3(const std::string& name, const GLfloat* value) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, value);
}

void Shader::setVec2(const std::string& name, const GLfloat* value) const
{
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, value);
}

void Shader::setVec2Array(const std::string& name, const GLsizei size, const GLfloat* value) const
{
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), size, value);
}
