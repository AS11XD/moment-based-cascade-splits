#include "Light.h"

void Light::genSphere(int slices, int stacks)
{
	//from: http://www.songho.ca/opengl/gl_sphere.html
	vert.reserve((slices + 1) * (stacks + 1));
	ind.reserve(slices * stacks);

	float x, y, z, xy;                              // vertex position
	float nx, ny, nz, lengthInv = 1.0f / radius;    // vertex normal
	float s, t;                                     // vertex texCoord

	float sectorStep = 2 * M_PI / slices;
	float stackStep = M_PI / stacks;
	float sectorAngle, stackAngle;

	for (int i = 0; i <= stacks; ++i)
	{
		stackAngle = M_PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
		xy = radius * cosf(stackAngle);       // r * cos(u)
		z = radius * sinf(stackAngle);              // r * sin(u)

		// add (sectorCount+1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coords
		for (int j = 0; j <= slices; ++j)
		{
			sectorAngle = j * sectorStep;           // starting from 0 to 2pi

			// vertex position (x, y, z)
			x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
			y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
			vert.push_back(x);
			vert.push_back(y);
			vert.push_back(z);

			// normalized vertex normal (nx, ny, nz)
			/*
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;
			vert.push_back(nx);
			vert.push_back(ny);
			vert.push_back(nz);
			*/
			vert.push_back(color.x);
			vert.push_back(color.y);
			vert.push_back(color.z);

			// vertex tex coord (s, t) range between [0, 1]
			s = (float)j / slices;
			t = (float)i / stacks;
			vert.push_back(s);
			vert.push_back(t);
		}
	}

	int k1, k2;
	for (int i = 0; i < stacks; ++i)
	{
		k1 = i * (slices + 1);     // beginning of current stack
		k2 = k1 + slices + 1;      // beginning of next stack

		for (int j = 0; j < slices; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding first and last stacks
			// k1 => k2 => k1+1
			if (i != 0)
			{
				ind.push_back(glm::ivec3(k1, k2, k1 + 1));
			}

			// k1+1 => k2 => k2+1
			if (i != (stacks - 1))
			{
				ind.push_back(glm::ivec3(k1 + 1, k2, k2 + 1));
			}
		}
	}
}

Light::Light(glm::ivec2 nAscpect, glm::vec3 nBoxColor, glm::vec3 nColor, float nRadius, int numberOfCascades, bool nWith_visualized_sm)
{
	color = nColor;
	radius = nRadius;
	MAX_CASCADES = numberOfCascades;
	for (int i = 0; i < MAX_CASCADES; i++) {
		cascades.push_back(new Camera(nAscpect, nBoxColor * (float)(i + 1) * 1.0f / (float)MAX_CASCADES));
	}

	glGenBuffers(1, &cs_matVP);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, cs_matVP);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * 16 * MAX_CASCADES, NULL, GL_STREAM_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, cs_matVP);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glGenBuffers(1, &splits);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, splits);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * 2 * MAX_CASCADES, NULL, GL_STREAM_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, splits);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	comp_matVP = new Shader("csm_compute_cascades.comp");

	with_visualized_sm = nWith_visualized_sm;
}

Light::~Light()
{
	for (Camera* c : cascades)
		c->~Camera();

	cascades.clear();

	vert.clear();
	ind.clear();

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ebo);
	
	glDeleteBuffers(1, &cs_matVP);
	glDeleteBuffers(1, &splits);

	comp_matVP->~Shader();
	free(comp_matVP);
}

void Light::initialize()
{
	for (Camera* c : cascades) {
		c->initialize();
		c->attachShadowTexture();
		if (with_visualized_sm) {
			c->attachDepthTexture();
		}
	}

	genSphere(64, 64);

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vert.size() * sizeof(float), vert.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind.size() * sizeof(int) * 3, ind.data(), GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
}

void Light::drawLight(Shader* s)
{
	s->setVec3("pos", glm::value_ptr(pos));
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, ind.size() * 3, GL_UNSIGNED_INT, 0);
}

void Light::drawBox(Shader* s)
{
	for (int i = 0; i < cascades.size(); i++) {
		cascades[i]->drawBox(s, i);
	}
}

void Light::activateTexturesVisualization(Shader* s, unsigned int fromTexture, int number)
{
	if (with_visualized_sm) {
		s->setInt("MAX_CASCADES", MAX_CASCADES);
		for (int i = 0; i < MAX_CASCADES; i++) {
			glActiveTexture(fromTexture + i);
			cascades[i]->bindTextureDepth();
			s->setInt("sm_light_vis[" + std::to_string(i) + "]", number + i);
		}
	}
}

void Light::activateTextures(Shader* s, unsigned int fromTexture, int number)
{
	std::vector<glm::mat4x4> mat;
	for (int i = 0; i < MAX_CASCADES; i++)
		mat.push_back(cascades[i]->getMatVP());
	
	std::vector<glm::vec2> one_over_asp;
	std::vector<glm::vec2> asp;
	for (int i = 0; i < MAX_CASCADES; i++) {
		asp.push_back(cascades[i]->getAspect());
		one_over_asp.push_back(glm::vec2(1.0f / (float)cascades[i]->getAspect().x, 1.0f / (float)cascades[i]->getAspect().y));
	}

	s->setVec2Array("one_over_aspect", one_over_asp.size(), glm::value_ptr(one_over_asp[0]));

	s->setVec2Array("aspect", one_over_asp.size(), glm::value_ptr(asp[0]));
	
	s->setVec3("sunCol", glm::value_ptr(color));

	s->setInt("MAX_CASCADES", MAX_CASCADES);

	for (int i = 0; i < MAX_CASCADES; i++) {
		glActiveTexture(fromTexture + i);
		cascades[i]->bindTextureShadow();
		s->setInt("sm_light[" + std::to_string(i) + "]", number + i);
	}
}

void Light::performPrePass(std::vector<std::shared_ptr<GLObjModel>> obj, glm::mat4x4& matM, Shader* s, bool visualize_sm)
{
	for (int i = 0; i < MAX_CASCADES; i++) {
		cascades[i]->performPrePassS(obj, matM, s, i);
	}

	if (with_visualized_sm && visualize_sm) {
		for (int i = 0; i < MAX_CASCADES; i++) {
			cascades[i]->performPrePassD(obj, matM, s, i);
		}
	}
}

void Light::updateCameraView(glm::vec3 nPos, glm::vec3 nDir, glm::vec3 nUp)
{
	pos = nPos;
	dir = glm::normalize(nDir);
	up = nUp;
	for (Camera* c : cascades)
		c->updateCameraView(nPos, nDir, nUp);
}

void Light::updateAspect(int w, int h)
{
	for (Camera* c : cascades) {
		c->updateAspect(w, h);

		c->attachShadowTexture();
		if (with_visualized_sm) {
			c->attachDepthTexture();
		}
	}
}

void Light::updateAspect(glm::ivec2 asp)
{
	for (Camera* c : cascades) {
		c->updateAspect(asp);

		c->attachShadowTexture();
		if (with_visualized_sm) {
			c->attachDepthTexture();
		}
	}
}

glm::ivec2 Light::getAspect()
{
	return cascades[0]->getAspect();
}

void Light::updateCameraProjection(float left, float right, float bottom, float top, float zNear, float zFar)
{
	for (Camera* c : cascades)
		c->updateCameraProjection(left, right, bottom, top, zNear, zFar);
}

void Light::updateCameraProjection(Camera* nc, std::vector<std::vector<GLuint>>* bf, std::vector<std::vector<GLuint>>* bf2, bool bbfit, float overlap)
{
	comp_matVP->use();
	if (bbfit) {
		std::vector<int> idf, idf2;
		for (int j = 0; j < MAX_CASCADES; j++) {
			glActiveTexture(GL_TEXTURE1 + j);
			glBindTexture(GL_TEXTURE_2D, bf->at(bf->size() - 1).at(j));
			idf.push_back(1 + j);
			glActiveTexture(GL_TEXTURE1 + j + MAX_CASCADES);
			glBindTexture(GL_TEXTURE_2D, bf2->at(bf2->size() - 1).at(j));
			idf2.push_back(1 + j + MAX_CASCADES);
		}
		comp_matVP->setIntVec("boundingBoxFit", idf.size(), idf.data());
		comp_matVP->setIntVec("boundingBoxFit2", idf2.size(), idf2.data());
	}
	comp_matVP->setBool("bbfit", bbfit);
	comp_matVP->setMatrix4x4("camP", glm::value_ptr(nc->getMatP()));
	comp_matVP->setMatrix4x4("camV", glm::value_ptr(nc->getMatV()));
	comp_matVP->setFloat("zNear", nc->getZNear());
	comp_matVP->setFloat("zFar", nc->getZFar());
	comp_matVP->setFloat("overlap", overlap);
	comp_matVP->setInt("MAX_CASCADES", MAX_CASCADES);
	comp_matVP->setVec3("pos", glm::value_ptr(pos));
	comp_matVP->setVec3("dir", glm::value_ptr(dir));
	comp_matVP->setVec3("up", glm::value_ptr(up));
	comp_matVP->setVec2("one_over_aspect", glm::value_ptr(glm::vec2(1.0 / cascades[0]->getAspect().x, 1.0 / cascades[0]->getAspect().y)));
	comp_matVP->use(1, 1, 1);
	comp_matVP->memoryBarrier();
}

std::vector<glm::vec2> Light::getSplits()
{
	std::vector<glm::vec2> spl;
	spl.reserve(MAX_CASCADES);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, splits);

	GLfloat* ptr;
	ptr = (GLfloat*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	
	for (int i = 0; i < MAX_CASCADES * 2; i += 2) {
		glm::vec2 split = glm::vec2(ptr[i], ptr[i + 1]);
		spl.push_back(split);
	}

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);	

	return spl;
}
