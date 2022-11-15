#include "Camera.h"

Camera::Camera(glm::ivec2 nAspect, glm::vec3 nBoxColor)
{
	aspect = nAspect;
	boxColor = nBoxColor;
}

Camera::~Camera()
{
	glDeleteVertexArrays(1, &vaoCube);
	glDeleteBuffers(1, &vboCube);
	glDeleteBuffers(1, &eboCube);
	vertCube.clear();
	indCube.clear();

	glDeleteFramebuffers(1, &fbo);
	glDeleteTextures(1, &tex_depth);

	glDeleteFramebuffers(1, &fbo_s);
	glDeleteTextures(1, &tex_depth_s);
}

void Camera::genUnitCube()
{
	vertCube.reserve(48);
	vertCube.push_back(-1.0f);
	vertCube.push_back(-1.0f);
	vertCube.push_back(-1.0f);

	vertCube.push_back(boxColor.x);
	vertCube.push_back(boxColor.y);
	vertCube.push_back(boxColor.z);


	vertCube.push_back(1.0f);
	vertCube.push_back(-1.0f);
	vertCube.push_back(-1.0f);

	vertCube.push_back(boxColor.x);
	vertCube.push_back(boxColor.y);
	vertCube.push_back(boxColor.z);


	vertCube.push_back(-1.0f);
	vertCube.push_back(1.0f);
	vertCube.push_back(-1.0f);

	vertCube.push_back(boxColor.x);
	vertCube.push_back(boxColor.y);
	vertCube.push_back(boxColor.z);


	vertCube.push_back(-1.0f);
	vertCube.push_back(-1.0f);
	vertCube.push_back(1.0f);

	vertCube.push_back(boxColor.x);
	vertCube.push_back(boxColor.y);
	vertCube.push_back(boxColor.z);


	vertCube.push_back(1.0f);
	vertCube.push_back(1.0f);
	vertCube.push_back(-1.0f);

	vertCube.push_back(boxColor.x);
	vertCube.push_back(boxColor.y);
	vertCube.push_back(boxColor.z);


	vertCube.push_back(1.0f);
	vertCube.push_back(-1.0f);
	vertCube.push_back(1.0f);

	vertCube.push_back(boxColor.x);
	vertCube.push_back(boxColor.y);
	vertCube.push_back(boxColor.z);


	vertCube.push_back(-1.0f);
	vertCube.push_back(1.0f);
	vertCube.push_back(1.0f);

	vertCube.push_back(boxColor.x);
	vertCube.push_back(boxColor.y);
	vertCube.push_back(boxColor.z);


	vertCube.push_back(1.0f);
	vertCube.push_back(1.0f);
	vertCube.push_back(1.0f);

	vertCube.push_back(boxColor.x);
	vertCube.push_back(boxColor.y);
	vertCube.push_back(boxColor.z);


	indCube.reserve(12);
	indCube.push_back(glm::ivec3(0, 1, 3));
	indCube.push_back(glm::ivec3(1, 5, 3));
	
	indCube.push_back(glm::ivec3(1, 4, 5));
	indCube.push_back(glm::ivec3(5, 4, 7));

	indCube.push_back(glm::ivec3(4, 2, 7));
	indCube.push_back(glm::ivec3(7, 2, 6));

	indCube.push_back(glm::ivec3(2, 0, 6));
	indCube.push_back(glm::ivec3(6, 0, 3));

	indCube.push_back(glm::ivec3(6, 3, 7));
	indCube.push_back(glm::ivec3(7, 3, 5));

	indCube.push_back(glm::ivec3(2, 4, 0));
	indCube.push_back(glm::ivec3(0, 4, 1));
	
}

void Camera::initialize()
{
	genUnitCube();

	glGenVertexArrays(1, &vaoCube);
	glGenBuffers(1, &vboCube);
	glGenBuffers(1, &eboCube);

	glBindVertexArray(vaoCube);

	glBindBuffer(GL_ARRAY_BUFFER, vboCube);
	glBufferData(GL_ARRAY_BUFFER, vertCube.size() * sizeof(float), vertCube.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboCube);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indCube.size() * sizeof(int) * 3, indCube.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void Camera::updateCameraView(glm::vec3 nPos, glm::vec3 nDir, glm::vec3 nUp)
{
	pos = nPos;
	dir = glm::normalize(nDir);
	up = nUp; 
	
	matV = glm::lookAt(pos, pos - dir, up);
}

void Camera::updateCameraProjection(glm::mat4x4 proj, float nZNear, float nZFar)
{
	zNear = nZNear;
	zFar = nZFar;
	matP = proj;
}

void Camera::updateCameraProjection(float fov, float nZNear, float nZFar)
{
	zNear = nZNear;
	zFar = nZFar;
	matP = glm::perspective(glm::radians(fov), (float)aspect.x / (float)aspect.y, zNear, zFar);
}

void Camera::updateCameraProjection(float left, float right, float bottom, float top, float nZNear, float nZFar)
{
	zNear = nZNear;
	zFar = nZFar;
	matP = glm::ortho(left, right, bottom, top, zNear, zFar);
}

void Camera::drawBox(Shader* s)
{
	s->setMatrix4x4("invL_MVP", glm::value_ptr(glm::inverse(matP * matV)));
	glBindVertexArray(vaoCube);
	glDrawElements(GL_TRIANGLES, indCube.size() * 3, GL_UNSIGNED_INT, 0);
}

void Camera::drawBox(Shader* s, int idx)
{
	s->setInt("idx", idx);
	glBindVertexArray(vaoCube);
	glDrawElements(GL_TRIANGLES, indCube.size() * 3, GL_UNSIGNED_INT, 0);
}

void Camera::attachShadowTexture()
{
	glGenFramebuffers(1, &fbo_s);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_s);
	glGenTextures(1, &tex_depth_s);
	glBindTexture(GL_TEXTURE_2D, tex_depth_s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_ONE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_ONE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, aspect.x, aspect.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex_depth_s, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Camera::attachDepthTexture()
{
	if (!validatt) {
		glGenFramebuffers(1, &fbo);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glGenTextures(1, &tex_depth);
	glBindTexture(GL_TEXTURE_2D, tex_depth);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_ZERO);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_ZERO);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, aspect.x, aspect.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tex_depth, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	depthatt = true;
}

void Camera::attachValidTexture()
{
	if (!depthatt) {
		glGenFramebuffers(1, &fbo);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glGenTextures(1, &tex_valid);
	glBindTexture(GL_TEXTURE_2D, tex_valid);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_ZERO);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_ZERO);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, aspect.x, aspect.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_valid, 0);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	validatt = true;
}

void Camera::bindFboS()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_s);
}

void Camera::bindFboD()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void Camera::updateAspect(int w, int h)
{
	aspect = glm::ivec2(w, h);
}

void Camera::updateAspect(glm::ivec2 asp)
{
	aspect = asp;

}

void Camera::bindTextureDepth()
{
	glBindTexture(GL_TEXTURE_2D, tex_depth);
}

void Camera::bindTextureValid()
{
	glBindTexture(GL_TEXTURE_2D, tex_valid);
}

void Camera::bindTextureShadow()
{
	glBindTexture(GL_TEXTURE_2D, tex_depth_s);
}

void Camera::performPrePassD(std::vector<std::shared_ptr<GLObjModel>> obj, glm::mat4x4& matM, glm::mat3x3& matNrml, glm::vec3 lightDir, Shader* s, bool checkDiff)
{
	bindFboD();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	if (!validatt)
		glClear(GL_DEPTH_BUFFER_BIT);
	else 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, aspect.x, aspect.y);

	s->use();
	s->setMatrix4x4("MVP", glm::value_ptr(getMatVP() * matM));
	if (validatt) {
		s->setMatrix3x3("nrmlMat", glm::value_ptr(matNrml));
		s->setVec3("lightDir", glm::value_ptr(lightDir));
	}
	s->setBool("checkDiffuse", validatt && checkDiff);

	for (std::shared_ptr<GLObjModel> objLoader : obj) {
		if (!validatt)
			objLoader->draw();
		else
			objLoader->draw(s, true);
	}
}

void Camera::performPrePassS(std::vector<std::shared_ptr<GLObjModel>> obj, glm::mat4x4& matM, Shader* s)
{
	bindFboS();
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, aspect.x, aspect.y);

	s->use();
	s->setMatrix4x4("MVP", glm::value_ptr(getMatVP() * matM));
	s->setMatrix4x4("matM", glm::value_ptr(matM));

	for (std::shared_ptr<GLObjModel> objLoader : obj)
		objLoader->draw();
}

void Camera::performPrePassD(std::vector<std::shared_ptr<GLObjModel>> obj, glm::mat4x4& matM, Shader* s, int idx)
{
	bindFboD();
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, aspect.x, aspect.y);

	s->use();
	s->setInt("idx", idx);
	s->setMatrix4x4("matM", glm::value_ptr(matM));

	for (std::shared_ptr<GLObjModel> objLoader : obj)
		objLoader->draw();
}

void Camera::performPrePassS(std::vector<std::shared_ptr<GLObjModel>> obj, glm::mat4x4& matM, Shader* s, int idx)
{
	bindFboS();
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_DEPTH_BUFFER_BIT);

	glViewport(0, 0, aspect.x, aspect.y);

	s->use();
	s->setInt("idx", idx);
	s->setMatrix4x4("matM", glm::value_ptr(matM));

	for (std::shared_ptr<GLObjModel> objLoader : obj)
		objLoader->draw();
}

std::vector<Camera*> Camera::splitAt(std::vector<glm::vec2> splits)
{
	std::vector<Camera*> nCams;
	nCams.reserve(splits.size());
	for (int i = 0; i < splits.size(); i++) {
		Camera* c = new Camera(aspect, boxColor * ((float)(splits.size() - i) / splits.size()));
		glm::mat4x4 nmV = glm::mat4x4(matV);
		c->setMatV(nmV);
		glm::mat4x4 nmP = glm::mat4x4(0.0);
		float nNear = splits[i].x * (zFar - zNear) + zNear;
		float nFar = splits[i].y * (zFar - zNear) + zNear;
		nmP[0][0] = matP[0][0];
		nmP[1][1] = matP[1][1];
		nmP[2][2] = -(nFar + nNear) / (nFar - nNear);
		nmP[2][3] = matP[2][3];
		nmP[3][2] = -(2.0 * nFar * nNear) / (nFar - nNear);
		c->setMatP(nmP);
		nCams.push_back(c);
	}
	return nCams;
}

float Camera::LinearizeDepth(float depth)
{
	if (depth >= 1.0)
		return 1.0;
	float z = depth * 2.0 - 1.0; // back to NDC 
	return (((2.0 * zNear * zFar) / (zFar + zNear - z * (zFar - zNear))) - zNear) / (zFar - zNear);
}
