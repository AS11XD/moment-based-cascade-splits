#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <array>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "nv_dds.h"
#include "Shader.h"

// texture surface range in index buffer
struct GLObjModelSurface
{
	unsigned int startIndex;
	unsigned int endIndex;
	unsigned int gl_texKa = 0;
	unsigned int gl_texKd = 0;
	unsigned int gl_texKs = 0;
	unsigned int gl_texBump = 0;
	glm::vec3 kd, ks, ka;
};

unsigned int loadGLTexture(const std::string& filename, float gamma);
unsigned int texture_loadDDS(const char* path);
void flip_image(unsigned char* bytes, int width, int height, int bytes_per_pixel);

class GLObjModel
{
private:
	std::string directory;

public:
	unsigned int vao = 0, buf_vertex = 0, buf_normal = 0, buf_texcoord = 0, buf_index = 0;
	int num_indices = 0;
	int flags = 0;
	enum {
		DEFAULT = 0,
		ADJACENCY = 1 << 0,
	};

	std::vector<GLObjModelSurface> modelSurfaceRange;

	GLObjModel(const std::string& path, bool loadNormalMaps = false, bool loadTextures = true, int flags = DEFAULT);
	~GLObjModel();
	void draw(Shader* s = nullptr, bool justbump = false);
	void draw(int surface_range_idx);

	int index_count() const;
};
