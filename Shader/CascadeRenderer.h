#ifndef CASCADE_RENDERER_H
#define CASCADE_RENDERER_H
#define _USE_MATH_DEFINES

#include "Renderer.h"

#include <iostream>
#include <cmath>
#include <ctime>
#include "Input.h"
#include <functional>
#include "GLObjModel.h"
#include "Light.h"
#include <functional>
#include <thread>

#include "stb_image_write.h"

class CascadeRenderer :
	public Renderer
{
private:


	glm::mat4x4 matM, matMVP;
	glm::mat3x3 matNrml;
	glm::vec3 camPosition, camTarget, camUp;

	std::vector<std::shared_ptr<GLObjModel>> objLoaders;

	Shader* objS, * createShS, * createDeS, * visualS, * lightS, * vfrustumCS, * vfrustumS, * reduceSumCompS, * reduceSumCompSInOne, * reduceMinMaxCompS, * reduceMinMaxOnesCompS,
		* createSumTestCompS, * createMinMaxTestCompS, * vTestCompS, * calcSplitsFixedS, * calcSplitsMinMaxS, * calcSplitsMomentsS, * calcBoundingBoxFittingS;

	std::vector<float> smVert;
	std::vector<glm::ivec3> smInd;
	GLuint smVao, smVbo, smEbo, smFbo;

	std::vector<GLuint> reduceImg;
	std::vector<GLuint> reduceImg2;
	std::vector<GLuint> reduceImgMinMaxOnes;
	std::vector<std::vector<GLuint>> fitBoundingBox;
	std::vector<std::vector<GLuint>> fitBoundingBox2;

	GLuint testReduce;
	glm::ivec2 testReduceAspect = glm::ivec2(801, 599);

	Light* sun;
	Camera* mainCamera;
	std::vector<Camera*> dummys;

	float scObjects = 1.0f;
	float alpha;
	bool cs = false;
	bool show_sm = false;
	bool swap_sm = false;
	bool show_ct = false;
	bool swap_ct = false;
	bool show_casc = false;
	bool swap_casc = false;
	bool bbfitting_active = true;
	bool swap_bbfitting = false;

	bool swap_ls = false;
	bool pause_calc = false;
	bool swap_pause = false;

	bool use_picture_test = false;

	bool show_test_reduce = false;
	bool swap_test_reduce = false;

	bool show_fr = false;
	bool swap_fr = false;
	bool swap_screenshot = false;
	int screenshots_linearized_taken = 0;
	int screenshots_unlinearized_taken = 0;
	int screenshots_taken = 0;

	bool output_splits = false;
	bool invalidate_diffuse = true;
	bool one_reduce_only = false;
	bool show_fps = true;
	bool use_improved_reduce = true;

	float overlap_cascades = 0.0f;
	float moment_bias = 5e-5f;
	int sm_size = 512;

	bool swap_screenshot_screen = false;

	int pcf_kernel = 3;

	std::vector<std::function<void()>*> loadFuncs;
	std::vector<std::function<void()>*> saveFuncs;
	std::vector<std::function<void()>*> exportSc;
	std::vector<std::function<void()>*> swapalgorithm;
	std::function<void()> makeSc;
	std::vector<std::string> exportScNames;
	std::vector<std::string> swapalgorithmNames;

	void resizeTextures();

	enum class CascadeAlgorithm
	{
		FIXED_SPLITS = 0,
		MIN_MAX_SDSM = 1,
		MOMENT_BASED_SDSM = 2
	};

	CascadeAlgorithm cascAlg = CascadeAlgorithm::FIXED_SPLITS;

	bool move_sun = false;
	bool swap_sun = false;
	float speed = 0.005f;
	void genGrid(std::vector<float>& vert, std::vector<glm::ivec3>& ind, float sizeX, float sizeY, int mx, int my, std::function<float(float, float)> func);
	void initGrid(int seed);
	void loadSaveFile(int i);
	void saveSaveFile(int i);
	void exportScreenshot(bool linear);
	void makeScreenshot();

public:
	CascadeRenderer(int w, int h, int seed, MenuController* mc);
	virtual ~CascadeRenderer();
	virtual void resize();
	virtual void render();
	virtual void input();
	virtual void timer();
	virtual void reloadShader();
};

#endif // !CASCADE_RENDERER_H