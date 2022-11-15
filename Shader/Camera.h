#ifndef CAMERA_H
#define CAMERA_H

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
#include "GLObjModel.h"

class Camera
{
protected:
	glm::mat4x4 matV, matP;
	glm::vec3 pos, dir, up, boxColor;
	glm::ivec2 aspect;

	float zNear, zFar;
	float mfov = -1.0f;

	std::vector<float> vertCube;
	std::vector<glm::ivec3> indCube;
	GLuint vaoCube, vboCube, eboCube;
	GLuint fbo, tex_depth, tex_valid;
	GLuint fbo_s, tex_depth_s;
	bool depthatt = false;
	bool validatt = false;

	void genUnitCube();
public:
	Camera(glm::ivec2 nAscpect, glm::vec3 nBoxColor);
	~Camera();

	void initialize();

	void setMatV(glm::mat4x4 nMatV) { matV = nMatV; }
	void setMatP(glm::mat4x4 nMatP) { matP = nMatP; }

	glm::vec3 getDir() { return dir; }
	glm::vec3 getPos() { return pos; }
	glm::mat4x4 getMatV() { return matV; }
	glm::mat4x4 getMatP() { return matP; }
	glm::mat4x4 getMatVP() { return matP * matV; }
	glm::ivec2 getAspect() { return aspect; }

	virtual float getZNear() { return zNear; }
	virtual float getZFar() { return zFar; }

	void updateCameraView(glm::vec3 nPos, glm::vec3 nDir, glm::vec3 nUp);
	
	void updateCameraProjection(glm::mat4x4 proj, float nZNear, float nZFar);

	void updateCameraProjection(float fov, float nZNear, float nZFar);

	void updateCameraProjection(float left, float right, float bottom, float top, float nZNear, float nZFar);

	void drawBox(Shader* s);
	
	void drawBox(Shader* s, int idx);

	void attachShadowTexture();

	void attachDepthTexture();
	
	void attachValidTexture();

	void bindFboS();

	void bindFboD();

	void updateAspect(int w, int h);

	void updateAspect(glm::ivec2 asp);

	void bindTextureDepth();

	void bindTextureValid();

	void bindTextureShadow();

	void performPrePassD(std::vector<std::shared_ptr<GLObjModel>> obj, glm::mat4x4& matM, glm::mat3x3& matNrml, glm::vec3 lightDir, Shader* s, bool checkDiff);

	void performPrePassS(std::vector<std::shared_ptr<GLObjModel>> obj, glm::mat4x4& matM, Shader* s);

	void performPrePassD(std::vector<std::shared_ptr<GLObjModel>> obj, glm::mat4x4& matM, Shader* s, int idx);

	void performPrePassS(std::vector<std::shared_ptr<GLObjModel>> obj, glm::mat4x4& matM, Shader* s, int idx);

	std::vector<Camera*> splitAt(std::vector<glm::vec2> splits);

	float LinearizeDepth(float depth);
};

#endif // !CAMERA_H