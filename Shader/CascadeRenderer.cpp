#include "CascadeRenderer.h"

void CascadeRenderer::resizeTextures()
{
	mainCamera->updateAspect(windowWidth, windowHeight);
	mainCamera->attachDepthTexture();
	mainCamera->attachValidTexture();
}

void CascadeRenderer::genGrid(std::vector<float>& vert, std::vector<glm::ivec3>& ind, float sizeX, float sizeY, int mx, int my, std::function<float(float, float)> func)
{
	vert.reserve((size_t)mx * (size_t)my * 8);
	ind.reserve(((size_t)mx - 1) * ((size_t)my - 1) * 2);
	for (int y = 0; y < my; ++y)
	{
		for (int x = 0; x < mx; ++x)
		{
			float u = (float)x / (float)(mx - 1);
			float v = (float)y / (float)(my - 1);

			vert.push_back((u - 0.5f) * sizeX);
			vert.push_back((v - 0.5f) * sizeY);
			vert.push_back(func(u, v));

			vert.push_back(1.0f);
			vert.push_back(1.0f);
			vert.push_back(1.0f);

			vert.push_back(u);
			vert.push_back(v);
		}
	}
	for (int y = 0; y < my - 1; ++y)
	{
		for (int x = 0; x < mx - 1; ++x)
		{
			ind.push_back(glm::ivec3(x + mx * y, (x + 1) + mx * y, x + mx * (y + 1)));
			ind.push_back(glm::ivec3(x + mx * (y + 1), (x + 1) + mx * y, (x + 1) + mx * (y + 1)));
		}
	}
}

void CascadeRenderer::initGrid(int seed)
{

	//objLoaders.push_back(std::make_shared<GLObjModel>("crytek-sponza/sponza_triag.obj"));
	//scObjects = 0.1f;

	//objLoaders.push_back(std::make_shared<GLObjModel>("groudon/groudon.obj"));
	objLoaders.push_back(std::make_shared<GLObjModel>("floor/floor.obj"));
	objLoaders.push_back(std::make_shared<GLObjModel>("pillars/pillars.obj"));
	scObjects = 1.0f;

	//objLoaders.push_back(std::make_shared<GLObjModel>("sponza/sponza.obj"));
	//scObjects = 0.1f;

	//objLoaders.push_back(std::make_shared<GLObjModel>("bistro/bistroExterior.obj", false, false));
	//objLoaders.push_back(std::make_shared<GLObjModel>("bistro/bistroExterior.obj", true));
	//objLoaders.push_back(std::make_shared<GLObjModel>("bistro/bistroExterior.obj"));
	//objLoaders.push_back(std::make_shared<GLObjModel>("levi/levi.obj"));
	//objLoaders.push_back(std::make_shared<GLObjModel>("bistro/bistroInterior.obj"));
	//objLoaders.push_back(std::make_shared<GLObjModel>("bistro/bistroInterior_Wine.obj"));
	//scObjects = 1.0f;

	//objLoaders.push_back(std::make_shared<GLObjModel>("emeraldSquare/emeraldSquare.obj"));
	//scObjects = 1.0f;

	mainCamera->initialize();
	mainCamera->attachDepthTexture();
	mainCamera->attachValidTexture();

	sun->initialize();

	genGrid(smVert, smInd, 1.0, 1.0, 2, 2,
			[](float x, float y)
			{
				return 0.0f;
			});

	glGenVertexArrays(1, &smVao);
	glGenBuffers(1, &smVbo);
	glGenBuffers(1, &smEbo);

	glBindVertexArray(smVao);

	glBindBuffer(GL_ARRAY_BUFFER, smVbo);
	glBufferData(GL_ARRAY_BUFFER, smVert.size() * sizeof(float), smVert.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, smEbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, smInd.size() * sizeof(int) * 3, smInd.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	reduceImg.resize(4);
	reduceImg2.resize(4);
	reduceImgMinMaxOnes.resize(4);

	int k = 1;
	for (int i = 0; i < reduceImg.size(); i++)
	{
		glGenTextures(1, &reduceImg[i]);
		glBindTexture(GL_TEXTURE_2D, reduceImg[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_ZERO);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_ZERO);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 512 / k, 512 / k, 0, GL_RGBA, GL_FLOAT, NULL);

		glGenTextures(1, &reduceImg2[i]);
		glBindTexture(GL_TEXTURE_2D, reduceImg2[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_ZERO);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_ZERO);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 512 / k, 512 / k, 0, GL_RGBA, GL_FLOAT, NULL);

		glGenTextures(1, &reduceImgMinMaxOnes[i]);
		glBindTexture(GL_TEXTURE_2D, reduceImgMinMaxOnes[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_ZERO);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_ZERO);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 512 / k, 512 / k, 0, GL_RGBA, GL_FLOAT, NULL);
		k *= 8;
	}

	for (int i = 0; i < 4; i++)
	{
		std::vector<GLuint> n1;
		std::vector<GLuint> n2;
		n1.resize(sun->getMaxCascades());
		n2.resize(sun->getMaxCascades());
		fitBoundingBox.push_back(n1);
		fitBoundingBox2.push_back(n2);
	}

	k = 1;
	for (int i = 0; i < fitBoundingBox.size(); i++)
	{
		for (int j = 0; j < sun->getMaxCascades(); j++)
		{
			glGenTextures(1, &fitBoundingBox[i][j]);
			glBindTexture(GL_TEXTURE_2D, fitBoundingBox[i][j]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_ZERO);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_ZERO);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 512 / k, 512 / k, 0, GL_RGBA, GL_FLOAT, NULL);

			glGenTextures(1, &fitBoundingBox2[i][j]);
			glBindTexture(GL_TEXTURE_2D, fitBoundingBox2[i][j]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_ZERO);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_ZERO);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 512 / k, 512 / k, 0, GL_RGBA, GL_FLOAT, NULL);
		}
		k *= 8;
	}


	glGenTextures(1, &testReduce);
	glBindTexture(GL_TEXTURE_2D, testReduce);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_ZERO);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_ZERO);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	if (!use_picture_test)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, testReduceAspect.x, testReduceAspect.y, 0, GL_RGBA, GL_FLOAT, NULL);
	}
	else
	{
		int width, height, nrChannels;
		float* data = stbi_loadf("./screenshots/hold (7).hdr", &width, &height, &nrChannels, 4);
		if (data)
		{
			testReduceAspect = glm::ivec2(width, height);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, testReduceAspect.x, testReduceAspect.y, 0, GL_RGBA, GL_FLOAT, data);
		}
		else
		{
			std::cout << "error loading image: picture test\n";
		}
		stbi_image_free(data);
	}

	swapalgorithm.push_back(new std::function<void()>([&]() { cascAlg = CascadeAlgorithm::FIXED_SPLITS; }));
	swapalgorithmNames.push_back("Fixed");
	swapalgorithm.push_back(new std::function<void()>([&]() { cascAlg = CascadeAlgorithm::MIN_MAX_SDSM; }));
	swapalgorithmNames.push_back("MinMax");
	swapalgorithm.push_back(new std::function<void()>([&]() { cascAlg = CascadeAlgorithm::MOMENT_BASED_SDSM; }));
	swapalgorithmNames.push_back("Moments");
	menuController->addButtons(&swapalgorithmNames, &swapalgorithm, 0.8, 0.8, 0.8, 1.0);

	menuController->addISlider("Shadowmap res:", &sm_size, 1, 4096, 0.8, 0.8, 0.8, 1.0);
	menuController->addISlider("PCF kernel:", &pcf_kernel, 1, 32, 0.8, 0.8, 0.8, 1.0);
	menuController->addSlider("Camera speed:", &speed, 0.00001f, 0.012f, 0.8, 0.8, 0.8, 1.0);
	menuController->addSlider("Sun rotation:", &alpha, 0.0, 360.0f, 0.8, 0.8, 0.8, 1.0);
	menuController->addSlider("Overlap casc:", &overlap_cascades, 0.0, 0.2, 0.8, 0.8, 0.8, 1.0);
	menuController->addSlider("Moment bias:", &moment_bias, 0.0, 0.001, 0.8, 0.8, 0.8, 1.0);
	menuController->addCheckBox("Improv:", &use_improved_reduce, 0.8, 0.8, 0.8, 1.0);
	menuController->addCheckBox("Show timer:", &show_fps, 0.8, 0.8, 0.8, 1.0);
	menuController->addCheckBox("Sun auto rotation:", &move_sun, 0.8, 0.8, 0.8, 1.0);
	menuController->addCheckBox("Show shadow map:", &show_sm, 0.8, 0.8, 0.8, 1.0);
	menuController->addCheckBox("Show cascades:", &show_casc, 0.8, 0.8, 0.8, 1.0);
	menuController->addCheckBox("Activate bounding box fitting:", &bbfitting_active, 0.8, 0.8, 0.8, 1.0);
	menuController->addCheckBox("Pause calculation:", &pause_calc, 0.8, 0.8, 0.8, 1.0);
	menuController->addCheckBox("Show frusta:", &show_fr, 0.8, 0.8, 0.8, 1.0);
	menuController->addCheckBox("Output splits:", &output_splits, 0.8, 0.8, 0.8, 1.0);
	menuController->addCheckBox("Invalidate diffuse:", &invalidate_diffuse, 0.8, 0.8, 0.8, 1.0);
	menuController->addCheckBox("One reduce:", &one_reduce_only, 0.8, 0.8, 0.8, 1.0);

	for (int i = 1; i <= 9; i++)
	{
		loadFuncs.push_back(new std::function<void()>([&, i]() { loadSaveFile(i); }));
	}
	menuController->addButtons("L", &loadFuncs, 1, 0.8, 0.8, 0.8, 1.0);

	for (int i = 1; i <= 9; i++)
	{
		saveFuncs.push_back(new std::function<void()>([&, i]() { saveSaveFile(i); }));
	}
	menuController->addButtons("S", &saveFuncs, 1, 0.8, 0.8, 0.8, 1.0);

	exportSc.push_back(new std::function<void()>([&]() { exportScreenshot(false); }));
	exportScNames.push_back("Export");
	exportSc.push_back(new std::function<void()>([&]() { exportScreenshot(true); }));
	exportScNames.push_back("Ex. Lin.");
	menuController->addButtons(&exportScNames, &exportSc, 0.8, 0.8, 0.8, 1.0);

	makeSc = [&]() { makeScreenshot(); };
	menuController->addButton("Save screen", &makeSc, 0.8, 0.8, 0.8, 1.0, true);

	//saveFunc = [&](int i) { saveSaveFile(i); };
	//menuController->addButtonContainer("S", &saveFunc, 1, 9, 0.8, 0.8, 0.8, 1.0);

	/*
	menuController->addSlider("shadow bias:", &ababa2, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0);
	menuController->addSlider("shadow bias:", &ababa3, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0);
	menuController->addSlider("shadow bias:", &ababa4, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0);
	menuController->addSlider("shadow bias:", &ababa5, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0);
	menuController->addSlider("shadow bias:", &ababa6, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0);
	menuController->addSlider("shadow bias:", &ababa7, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0);
	menuController->addSlider("shadow bias:", &ababa8, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0);
	menuController->addSlider("shadow bias:", &ababa9, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0);
	menuController->addSlider("shadow bias:", &ababa01, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0);
	menuController->addSlider("shadow bias:", &ababa02, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0);
	menuController->addSlider("shadow bias:", &ababa03, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0);
	menuController->addSlider("shadow bias:", &ababa04, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0);
	menuController->addSlider("shadow bias:", &ababa05, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0);
	menuController->addSlider("shadow bias:", &ababa06, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0);
	menuController->addSlider("shadow bias:", &ababa07, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0);
	menuController->addSlider("shadow bias:", &ababa08, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0);
	menuController->addSlider("shadow bias:", &ababa09, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0);
	menuController->addSlider("shadow bias:", &ababa11, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0);
	menuController->addSlider("shadow bias:", &ababa12, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0);
	menuController->addSlider("shadow bias:", &ababa13, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0);
	menuController->addSlider("shadow bias:", &ababa14, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0);
	menuController->addSlider("shadow bias:", &ababa15, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0);
	menuController->addSlider("shadow bias:", &ababa16, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0);
	menuController->addSlider("shadow bias:", &ababa17, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0);
	menuController->addSlider("shadow bias:", &ababa18, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0);
	menuController->addSlider("shadow bias:", &ababa19, 0.0, 1.0, 0.0, 1.0, 1.0, 1.0);
	*/
}

CascadeRenderer::CascadeRenderer(int w, int h, int seed, MenuController* mc) : Renderer(w, h, seed, mc)
{
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);

	reloadShader();
	camPosition = glm::vec3(1.0f, 1.0f, 0.5f);
	camTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	camUp = glm::vec3(0.0f, 0.0f, 1.0f);

	mainCamera = new Camera(glm::ivec2(windowWidth, windowHeight), glm::vec3(0.0f, 1.0f, 0.0f));

	sun = new Light(glm::ivec2(sm_size, sm_size),
					glm::vec3(1.0f, 0.0f, 1.0f),
					glm::vec3(1.0f, 1.0f, 0.9f),
					0.1f, 4);

	initGrid(seed);
}

CascadeRenderer::~CascadeRenderer()
{
	glDisable(GL_DEPTH_TEST);

	sun->~Light();

	for (std::shared_ptr<GLObjModel> objLoader : objLoaders)
		objLoader->~GLObjModel();
	objLoaders.clear();

	objS->~Shader();
	free(objS);

	glDeleteVertexArrays(1, &smVao);
	glDeleteBuffers(1, &smVbo);
	glDeleteBuffers(1, &smEbo);

	for (int i = 0; i < reduceImg.size(); i++)
		glDeleteTextures(1, &reduceImg[i]);
	for (int i = 0; i < reduceImg2.size(); i++)
		glDeleteTextures(1, &reduceImg2[i]);
	for (int i = 0; i < reduceImgMinMaxOnes.size(); i++)
		glDeleteTextures(1, &reduceImgMinMaxOnes[i]);
	reduceImg.clear();
	reduceImg2.clear();
	reduceImgMinMaxOnes.clear();

	glDeleteTextures(1, &testReduce);

	createMinMaxTestCompS->~Shader();
	free(createMinMaxTestCompS);

	createSumTestCompS->~Shader();
	free(createSumTestCompS);

	vTestCompS->~Shader();
	free(vTestCompS);

	reduceSumCompS->~Shader();
	free(reduceSumCompS);

	reduceSumCompSInOne->~Shader();
	free(reduceSumCompSInOne);

	reduceMinMaxCompS->~Shader();
	free(reduceMinMaxCompS);

	reduceMinMaxOnesCompS->~Shader();
	free(reduceMinMaxOnesCompS);

	calcSplitsFixedS->~Shader();
	free(calcSplitsFixedS);

	calcSplitsMinMaxS->~Shader();
	free(calcSplitsMinMaxS);

	calcSplitsMomentsS->~Shader();
	free(calcSplitsMomentsS);

	calcBoundingBoxFittingS->~Shader();
	free(calcBoundingBoxFittingS);

	createShS->~Shader();
	free(createShS);

	createDeS->~Shader();
	free(createDeS);

	visualS->~Shader();
	free(visualS);

	lightS->~Shader();
	free(lightS);

	vfrustumS->~Shader();
	free(vfrustumS);

	vfrustumCS->~Shader();
	free(vfrustumCS);

	smVert.clear();
	smInd.clear();

	saveFuncs.clear();
	loadFuncs.clear();
}

void CascadeRenderer::resize()
{
	resizeTextures();
	cs = false;
}

void CascadeRenderer::render()
{
	endFPSCount();
	startFPSCount();

	{ //prepass
		if (!pause_calc)
		{

			mainCamera->performPrePassD(objLoaders, matM, matNrml, sun->getDir(), createDeS, invalidate_diffuse);

			{ // reduce
				float clear_color_sum[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
				float clear_color_min_max_ones[4] = { std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), 0.0f, 0.0f };
				float clear_color_fit_bbf[4] = { std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(),
												 std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity() };
				for (int i = 0; i < reduceImg.size(); i++)
				{
					if (cascAlg == CascadeAlgorithm::MOMENT_BASED_SDSM)
					{
						glClearTexImage(reduceImg[i], 0, GL_RGBA, GL_FLOAT, &clear_color_sum);
						glClearTexImage(reduceImg2[i], 0, GL_RGBA, GL_FLOAT, &clear_color_sum);
					}
					else
					{
						glClearTexImage(reduceImg[i], 0, GL_RGBA, GL_FLOAT, &clear_color_min_max_ones);
						glClearTexImage(reduceImg2[i], 0, GL_RGBA, GL_FLOAT, &clear_color_min_max_ones);
					}
					glClearTexImage(reduceImgMinMaxOnes[i], 0, GL_RGBA, GL_FLOAT, &clear_color_min_max_ones);
				}

				for (int i = 0; i < fitBoundingBox.size(); i++)
				{
					for (int j = 0; j < fitBoundingBox[i].size(); j++)
					{
						glClearTexImage(fitBoundingBox[i][j], 0, GL_RGBA, GL_FLOAT, &clear_color_fit_bbf);
						glClearTexImage(fitBoundingBox2[i][j], 0, GL_RGBA, GL_FLOAT, &clear_color_fit_bbf);
					}
				}

				if (show_test_reduce)
				{
					if (!use_picture_test)
					{
						switch (cascAlg)
						{
							case CascadeAlgorithm::MOMENT_BASED_SDSM:
								createSumTestCompS->use();
								glBindImageTexture(0, testReduce, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
								createSumTestCompS->setInt("testImg", 0);
								createSumTestCompS->setVec2("aspect", glm::value_ptr(glm::vec2(testReduceAspect)));
								glm::ivec2 pos;
								pos = glm::ivec2(rand() % testReduceAspect.x, rand() % testReduceAspect.y);
								createSumTestCompS->setVec2("pos", glm::value_ptr(glm::vec2(pos)));
								createSumTestCompS->use(testReduceAspect.x, testReduceAspect.y, 1);
								createSumTestCompS->memoryBarrier();
								break;

							case CascadeAlgorithm::MIN_MAX_SDSM:

								createMinMaxTestCompS->use();
								glBindImageTexture(0, testReduce, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
								createMinMaxTestCompS->setInt("testImg", 0);
								createMinMaxTestCompS->setVec2("aspect", glm::value_ptr(glm::vec2(testReduceAspect)));
								glm::ivec2 posMin, posMax;
								posMin = glm::ivec2(rand() % testReduceAspect.x, rand() % testReduceAspect.y);
								do
								{
									posMax = glm::ivec2(rand() % testReduceAspect.x, rand() % testReduceAspect.y);
								} while (posMin.x == posMax.x && posMin.y == posMax.y);
								createMinMaxTestCompS->setVec2("posMin", glm::value_ptr(glm::vec2(posMin)));
								createMinMaxTestCompS->setVec2("posMax", glm::value_ptr(glm::vec2(posMax)));
								createMinMaxTestCompS->use(testReduceAspect.x, testReduceAspect.y, 1);
								createMinMaxTestCompS->memoryBarrier();

								break;

						}
					}
				}

				int k = 1;
				switch (cascAlg)
				{
					case CascadeAlgorithm::MOMENT_BASED_SDSM:
						if (!one_reduce_only)
						{
							k = 1;
							for (int i = 0; i < reduceImgMinMaxOnes.size(); i++)
							{
								reduceMinMaxOnesCompS->use();
								glBindImageTexture(0, reduceImgMinMaxOnes[i], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
								reduceMinMaxOnesCompS->setInt("reducedImg", 0);

								if (i == 0)
								{
									glActiveTexture(GL_TEXTURE1);
									if (!show_test_reduce)
									{
										mainCamera->bindTextureDepth();

										glActiveTexture(GL_TEXTURE2);
										mainCamera->bindTextureValid();
										reduceMinMaxOnesCompS->setInt("validity", 2);

										reduceMinMaxOnesCompS->setVec2("one_over_aspect", glm::value_ptr(glm::vec2(1.0f / (mainCamera->getAspect().x), 1.0f / (mainCamera->getAspect().y))));
										reduceMinMaxOnesCompS->setVec2("aspect", glm::value_ptr(glm::vec2(mainCamera->getAspect())));
									}
									else
									{
										glBindTexture(GL_TEXTURE_2D, testReduce);
										reduceMinMaxOnesCompS->setVec2("one_over_aspect", glm::value_ptr(glm::vec2(1.0f / (testReduceAspect.x), 1.0f / (testReduceAspect.y))));
										reduceMinMaxOnesCompS->setVec2("aspect", glm::value_ptr(glm::vec2(testReduceAspect)));
									}
									reduceMinMaxOnesCompS->setFloat("near", mainCamera->getZNear());
									reduceMinMaxOnesCompS->setFloat("far", mainCamera->getZFar());
									reduceMinMaxOnesCompS->setFloat("height", (mainCamera->getPos().z) / (mainCamera->getZFar() - mainCamera->getZNear()));
									reduceMinMaxOnesCompS->setBool("test", show_test_reduce);
									reduceMinMaxOnesCompS->setBool("firstIteration", true);
								}
								else
								{
									glActiveTexture(GL_TEXTURE1);
									glBindTexture(GL_TEXTURE_2D, reduceImgMinMaxOnes[i - 1]);
									reduceMinMaxOnesCompS->setVec2("one_over_aspect", glm::value_ptr(glm::vec2(1.0f / (4096.0f / (float)k), 1.0f / (4096.0f / (float)k))));
									reduceMinMaxOnesCompS->setVec2("aspect", glm::value_ptr(glm::vec2((4096.0f / (float)k))));
									reduceMinMaxOnesCompS->setBool("firstIteration", false);
								}
								reduceMinMaxOnesCompS->setInt("depth", 1);

								if (use_improved_reduce)
								{
									int offsX = (mainCamera->getAspect().x % (8 * k) == 0) ? 0 : 1;
									int offsY = (mainCamera->getAspect().y % (8 * k) == 0) ? 0 : 1;
									reduceMinMaxOnesCompS->use((mainCamera->getAspect().x / (8 * k)) + offsX, (mainCamera->getAspect().y / (8 * k)) + offsY, 1);
								}
								else
								{
									reduceMinMaxOnesCompS->use(512 / k, 512 / k, 1);
								}
								reduceMinMaxOnesCompS->memoryBarrier();
								k *= 8;
							}

							k = 1;
							for (int i = 0; i < reduceImg.size(); i++)
							{
								reduceSumCompS->use();
								glBindImageTexture(0, reduceImg[i], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
								reduceSumCompS->setInt("reducedImg", 0);

								glBindImageTexture(1, reduceImg2[i], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
								reduceSumCompS->setInt("reducedImg2", 1);

								if (i == 0)
								{
									glActiveTexture(GL_TEXTURE2);
									if (!show_test_reduce)
									{
										mainCamera->bindTextureDepth();

										glActiveTexture(GL_TEXTURE5);
										mainCamera->bindTextureValid();
										reduceSumCompS->setInt("validity", 5);

										reduceSumCompS->setVec2("one_over_aspect", glm::value_ptr(glm::vec2(1.0f / (mainCamera->getAspect().x), 1.0f / (mainCamera->getAspect().y))));
										reduceSumCompS->setVec2("aspect", glm::value_ptr(glm::vec2(mainCamera->getAspect())));
										reduceSumCompS->setFloat("near", mainCamera->getZNear());
										reduceSumCompS->setFloat("far", mainCamera->getZFar());
										reduceSumCompS->setFloat("height", (mainCamera->getPos().z) / (mainCamera->getZFar() - mainCamera->getZNear()));
										reduceSumCompS->setBool("test", show_test_reduce);
									}
									else
									{
										glBindTexture(GL_TEXTURE_2D, testReduce);
										reduceSumCompS->setVec2("one_over_aspect", glm::value_ptr(glm::vec2(1.0f / (testReduceAspect.x), 1.0f / (testReduceAspect.y))));
										reduceSumCompS->setVec2("aspect", glm::value_ptr(glm::vec2(testReduceAspect)));
										reduceSumCompS->setBool("test", show_test_reduce);
									}
									reduceSumCompS->setBool("firstIteration", true);


									glActiveTexture(GL_TEXTURE4);
									glBindTexture(GL_TEXTURE_2D, reduceImgMinMaxOnes[reduceImgMinMaxOnes.size() - 1]);
									reduceSumCompS->setInt("minMaxOnes", 4);

								}
								else
								{
									glActiveTexture(GL_TEXTURE2);
									glBindTexture(GL_TEXTURE_2D, reduceImg[i - 1]);

									glActiveTexture(GL_TEXTURE3);
									glBindTexture(GL_TEXTURE_2D, reduceImg2[i - 1]);

									reduceSumCompS->setVec2("one_over_aspect", glm::value_ptr(glm::vec2(1.0f / (4096.0f / (float)k), 1.0f / (4096.0f / (float)k))));
									reduceSumCompS->setVec2("aspect", glm::value_ptr(glm::vec2((4096.0f / (float)k))));
									reduceSumCompS->setBool("firstIteration", false);
								}
								reduceSumCompS->setInt("depth", 2);
								reduceSumCompS->setInt("depth2", 3);

								if (use_improved_reduce)
								{
									int offsX = (mainCamera->getAspect().x % (8 * k) == 0) ? 0 : 1;
									int offsY = (mainCamera->getAspect().y % (8 * k) == 0) ? 0 : 1;
									reduceSumCompS->use((mainCamera->getAspect().x / (8 * k)) + offsX, (mainCamera->getAspect().y / (8 * k)) + offsY, 1);
								}
								else
								{
									reduceSumCompS->use(512 / k, 512 / k, 1);
								}
								reduceSumCompS->memoryBarrier();
								k *= 8;
							}
						}
						else
						{
							k = 1;
							for (int i = 0; i < reduceImg.size(); i++)
							{
								reduceSumCompSInOne->use();
								glBindImageTexture(0, reduceImg[i], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
								reduceSumCompSInOne->setInt("reducedImg", 0);

								glBindImageTexture(1, reduceImg2[i], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
								reduceSumCompSInOne->setInt("reducedImg2", 1);

								glBindImageTexture(2, reduceImgMinMaxOnes[i], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
								reduceSumCompSInOne->setInt("reducedImg3", 2);


								if (i == 0)
								{
									glActiveTexture(GL_TEXTURE3);
									if (!show_test_reduce)
									{
										mainCamera->bindTextureDepth();

										glActiveTexture(GL_TEXTURE6);
										mainCamera->bindTextureValid();
										reduceSumCompSInOne->setInt("validity", 6);

										reduceSumCompSInOne->setVec2("one_over_aspect", glm::value_ptr(glm::vec2(1.0f / (mainCamera->getAspect().x), 1.0f / (mainCamera->getAspect().y))));
										reduceSumCompSInOne->setVec2("aspect", glm::value_ptr(glm::vec2(mainCamera->getAspect())));
										reduceSumCompSInOne->setFloat("near", mainCamera->getZNear());
										reduceSumCompSInOne->setFloat("far", mainCamera->getZFar());
										reduceSumCompSInOne->setFloat("height", (mainCamera->getPos().z) / (mainCamera->getZFar() - mainCamera->getZNear()));
										reduceSumCompSInOne->setBool("test", show_test_reduce);
									}
									else
									{
										glBindTexture(GL_TEXTURE_2D, testReduce);
										reduceSumCompSInOne->setVec2("one_over_aspect", glm::value_ptr(glm::vec2(1.0f / (testReduceAspect.x), 1.0f / (testReduceAspect.y))));
										reduceSumCompSInOne->setVec2("aspect", glm::value_ptr(glm::vec2(testReduceAspect)));
										reduceSumCompSInOne->setBool("test", show_test_reduce);
									}
									reduceSumCompSInOne->setBool("firstIteration", true);
								}
								else
								{
									glActiveTexture(GL_TEXTURE3);
									glBindTexture(GL_TEXTURE_2D, reduceImg[i - 1]);

									glActiveTexture(GL_TEXTURE4);
									glBindTexture(GL_TEXTURE_2D, reduceImg2[i - 1]);

									glActiveTexture(GL_TEXTURE5);
									glBindTexture(GL_TEXTURE_2D, reduceImgMinMaxOnes[i - 1]);

									reduceSumCompSInOne->setVec2("one_over_aspect", glm::value_ptr(glm::vec2(1.0f / (4096.0f / (float)k), 1.0f / (4096.0f / (float)k))));
									reduceSumCompSInOne->setVec2("aspect", glm::value_ptr(glm::vec2((4096.0f / (float)k))));
									reduceSumCompSInOne->setBool("firstIteration", false);
								}
								reduceSumCompSInOne->setInt("depth", 3);
								reduceSumCompSInOne->setInt("depth2", 4);
								reduceSumCompSInOne->setInt("depth3", 5);

								if (use_improved_reduce)
								{
									int offsX = (mainCamera->getAspect().x % (8 * k) == 0) ? 0 : 1;
									int offsY = (mainCamera->getAspect().y % (8 * k) == 0) ? 0 : 1;
									reduceSumCompSInOne->use((mainCamera->getAspect().x / (8 * k)) + offsX, (mainCamera->getAspect().y / (8 * k)) + offsY, 1);
								}
								else
								{
									reduceSumCompSInOne->use(512 / k, 512 / k, 1);
								}
								reduceSumCompSInOne->memoryBarrier();
								k *= 8;
							}
						}
						break;

					case CascadeAlgorithm::MIN_MAX_SDSM:
						k = 1;
						for (int i = 0; i < reduceImg.size(); i++)
						{
							reduceMinMaxCompS->use();
							glBindImageTexture(0, reduceImg[i], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
							reduceMinMaxCompS->setInt("reducedImg", 0);

							if (i == 0)
							{
								glActiveTexture(GL_TEXTURE1);
								if (!show_test_reduce)
								{
									mainCamera->bindTextureDepth();

									glActiveTexture(GL_TEXTURE2);
									mainCamera->bindTextureValid();
									reduceMinMaxCompS->setInt("validity", 2);

									reduceMinMaxCompS->setFloat("near", mainCamera->getZNear());
									reduceMinMaxCompS->setFloat("far", mainCamera->getZFar());
									reduceMinMaxCompS->setBool("test", show_test_reduce);
									reduceMinMaxCompS->setVec2("one_over_aspect", glm::value_ptr(glm::vec2(1.0f / (mainCamera->getAspect().x), 1.0f / (mainCamera->getAspect().y))));
									reduceMinMaxCompS->setVec2("aspect", glm::value_ptr(glm::vec2(mainCamera->getAspect())));
								}
								else
								{
									glBindTexture(GL_TEXTURE_2D, testReduce);
									reduceMinMaxCompS->setVec2("one_over_aspect", glm::value_ptr(glm::vec2(1.0f / (testReduceAspect.x), 1.0f / (testReduceAspect.y))));
									reduceMinMaxCompS->setVec2("aspect", glm::value_ptr(glm::vec2(testReduceAspect)));
									reduceMinMaxCompS->setBool("test", show_test_reduce);
								}
								reduceMinMaxCompS->setBool("firstIteration", true);
							}
							else
							{
								glActiveTexture(GL_TEXTURE1);
								glBindTexture(GL_TEXTURE_2D, reduceImg[i - 1]);

								reduceMinMaxCompS->setVec2("one_over_aspect", glm::value_ptr(glm::vec2(1.0f / (4096.0f / (float)k), 1.0f / (4096.0f / (float)k))));
								reduceMinMaxCompS->setVec2("aspect", glm::value_ptr(glm::vec2((4096.0f / (float)k))));
								reduceMinMaxCompS->setBool("firstIteration", false);
							}
							reduceMinMaxCompS->setInt("depth", 1);

							if (use_improved_reduce)
							{
								int offsX = (mainCamera->getAspect().x % (8 * k) == 0) ? 0 : 1;
								int offsY = (mainCamera->getAspect().y % (8 * k) == 0) ? 0 : 1;
								reduceMinMaxCompS->use((mainCamera->getAspect().x / (8 * k)) + offsX, (mainCamera->getAspect().y / (8 * k)) + offsY, 1);
							}
							else
							{
								reduceMinMaxCompS->use(512 / k, 512 / k, 1);
							}
							reduceMinMaxCompS->memoryBarrier();
							k *= 8;
						}
						break;
				}
			}

			{ // calc splits

				std::vector<glm::vec2> splits;
				splits.push_back(glm::vec2(0.0f, 0.125f));
				splits.push_back(glm::vec2(0.125f, 0.25f));
				splits.push_back(glm::vec2(0.25f, 0.5f));
				splits.push_back(glm::vec2(0.5f, 1.0f));

				switch (cascAlg)
				{
					case CascadeAlgorithm::FIXED_SPLITS:
						calcSplitsFixedS->use();
						calcSplitsFixedS->setInt("MAX_CASCADES", sun->getMaxCascades());
						calcSplitsFixedS->setVec2Array("spl", splits.size(), glm::value_ptr(splits[0]));
						calcSplitsFixedS->use(1, 1, 1);
						calcSplitsFixedS->memoryBarrier();
						break;

					case CascadeAlgorithm::MOMENT_BASED_SDSM:
						calcSplitsMomentsS->use();
						calcSplitsMomentsS->setInt("MAX_CASCADES", sun->getMaxCascades());
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, reduceImg[reduceImg.size() - 1]);
						calcSplitsMomentsS->setInt("reducedImg", 0);
						glActiveTexture(GL_TEXTURE1);
						glBindTexture(GL_TEXTURE_2D, reduceImg2[reduceImg2.size() - 1]);
						calcSplitsMomentsS->setInt("reducedImg2", 1);
						glActiveTexture(GL_TEXTURE2);
						glBindTexture(GL_TEXTURE_2D, reduceImgMinMaxOnes[reduceImgMinMaxOnes.size() - 1]);
						calcSplitsMomentsS->setInt("reducedImgMinMaxOnes", 2);
						calcSplitsMomentsS->setVec2("aspect", glm::value_ptr(glm::vec2(mainCamera->getAspect())));
						calcSplitsMomentsS->setFloat("near", mainCamera->getZNear());
						calcSplitsMomentsS->setFloat("far", mainCamera->getZFar());
						calcSplitsMomentsS->setFloat("moment_bias", moment_bias);
						calcSplitsMomentsS->setBool("one_reduce_only", one_reduce_only);
						calcSplitsMomentsS->setFloat("height", (mainCamera->getPos().z) / (mainCamera->getZFar() - mainCamera->getZNear()));
						calcSplitsMomentsS->use(1, 1, 1);
						calcSplitsMomentsS->memoryBarrier();
						break;

					case CascadeAlgorithm::MIN_MAX_SDSM:
						calcSplitsMinMaxS->use();
						calcSplitsMinMaxS->setInt("MAX_CASCADES", sun->getMaxCascades());
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, reduceImg[reduceImg.size() - 1]);
						calcSplitsMinMaxS->setInt("reducedImg", 0);
						calcSplitsMinMaxS->setFloat("near", mainCamera->getZNear());
						calcSplitsMinMaxS->setFloat("far", mainCamera->getZFar());
						calcSplitsMinMaxS->use(1, 1, 1);
						calcSplitsMinMaxS->memoryBarrier();
						break;
				}


			}

			if (bbfitting_active)
			{ //fit boundingboxes
				int k = 1;
				for (int i = 0; i < fitBoundingBox.size(); i++)
				{
					calcBoundingBoxFittingS->use();
					std::vector<int> idf, idf2;
					for (int j = 0; j < sun->getMaxCascades(); j++)
					{
						glBindImageTexture(j, fitBoundingBox[i][j], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
						idf.push_back(j);
						glBindImageTexture(j + sun->getMaxCascades(), fitBoundingBox2[i][j], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
						idf2.push_back(j + sun->getMaxCascades());
					}
					calcBoundingBoxFittingS->setIntVec("reducedImg", idf.size(), idf.data());
					calcBoundingBoxFittingS->setIntVec("reducedImg2", idf2.size(), idf2.data());


					std::vector<int> nidf, nidf2;
					if (i == 0)
					{
						glActiveTexture(GL_TEXTURE1);
						nidf.push_back(1);
						mainCamera->bindTextureDepth();
						calcBoundingBoxFittingS->setFloat("near", mainCamera->getZNear());
						calcBoundingBoxFittingS->setFloat("far", mainCamera->getZFar());
						calcBoundingBoxFittingS->setVec2("one_over_aspect", glm::value_ptr(glm::vec2(1.0f / (mainCamera->getAspect().x), 1.0f / (mainCamera->getAspect().y))));
						calcBoundingBoxFittingS->setVec2("aspect", glm::value_ptr(glm::vec2(mainCamera->getAspect())));
						calcBoundingBoxFittingS->setBool("firstIteration", true);
					}
					else
					{
						for (int j = 0; j < sun->getMaxCascades(); j++)
						{
							glActiveTexture(GL_TEXTURE1 + j);
							glBindTexture(GL_TEXTURE_2D, fitBoundingBox[i - 1][j]);
							nidf.push_back(1 + j);
							glActiveTexture(GL_TEXTURE1 + j + sun->getMaxCascades());
							glBindTexture(GL_TEXTURE_2D, fitBoundingBox2[i - 1][j]);
							nidf2.push_back(1 + j + sun->getMaxCascades());
						}

						calcBoundingBoxFittingS->setVec2("one_over_aspect", glm::value_ptr(glm::vec2(1.0f / (4096.0f / (float)k), 1.0f / (4096.0f / (float)k))));
						calcBoundingBoxFittingS->setVec2("aspect", glm::value_ptr(glm::vec2((4096.0f / (float)k))));
						calcBoundingBoxFittingS->setBool("firstIteration", false);
					}
					calcBoundingBoxFittingS->setIntVec("depth", nidf.size(), nidf.data());
					calcBoundingBoxFittingS->setIntVec("depth2", nidf2.size(), nidf2.data());

					if (use_improved_reduce)
					{
						int offsX = (mainCamera->getAspect().x % (8 * k) == 0) ? 0 : 1;
						int offsY = (mainCamera->getAspect().y % (8 * k) == 0) ? 0 : 1;
						calcBoundingBoxFittingS->use((mainCamera->getAspect().x / (8 * k)) + offsX, (mainCamera->getAspect().y / (8 * k)) + offsY, 1);
					}
					else
					{
						calcBoundingBoxFittingS->use(512 / k, 512 / k, 1);
					}
					calcBoundingBoxFittingS->memoryBarrier();
					k *= 8;
				}
			}

			glm::vec3 pos = glm::vec3(2.4f * cos(alpha / 180.0f * M_PI), 2.4f * sin(alpha / 180.0f * M_PI), 3.8);
			sun->updateCameraView(pos, glm::normalize(pos), camUp);
			sun->updateCameraProjection(mainCamera, &fitBoundingBox, &fitBoundingBox2, bbfitting_active, overlap_cascades);
		}

		glEnable(GL_DEPTH_CLAMP);
		sun->performPrePass(objLoaders, matM, createShS, show_sm);
		glDisable(GL_DEPTH_CLAMP);
	}

	if (output_splits)
	{
		for (glm::vec2 sp : sun->getSplits())
		{
			std::cout << sp.x << ", " << sp.y << "\n";
		}
		std::cout << "\n\n";
	}

	{   //pass2
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.6f, 0.6f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glViewport(0, 0, windowWidth, windowHeight);

		objS->use();
		sun->activateTextures(objS, GL_TEXTURE4, 4);
		objS->setMatrix4x4("matM", glm::value_ptr(matM));
		objS->setMatrix4x4("MVP", glm::value_ptr(matMVP));
		objS->setMatrix3x3("nrmlMat", glm::value_ptr(matNrml));
		objS->setBool("showCasc", show_casc);
		objS->setInt("pcf_kernel", pcf_kernel);
		objS->setFloat("near", mainCamera->getZNear());
		objS->setFloat("far", mainCamera->getZFar());
		objS->setFloat("overlap", overlap_cascades);
		objS->setVec3("lightDir", glm::value_ptr(sun->getDir()));
		objS->setBool("bbfit", bbfitting_active);
		for (std::shared_ptr<GLObjModel> objLoader : objLoaders)
			objLoader->draw(objS);
	}

	{	//lightsource
		lightS->use();
		lightS->setMatrix4x4("MVP", glm::value_ptr(mainCamera->getMatVP()));
		sun->drawLight(lightS);
	}

	{	//visualization
		if (show_fr)
		{

			glDepthMask(GL_TRUE);

			vfrustumCS->use();
			vfrustumCS->setMatrix4x4("MVP", glm::value_ptr(mainCamera->getMatVP()));
			sun->drawBox(vfrustumCS);

			vfrustumS->use();
			vfrustumS->setMatrix4x4("MVP", glm::value_ptr(mainCamera->getMatVP()));
			for (Camera* c : dummys)
			{
				c->drawBox(vfrustumS);
			}
		}

		if (show_sm)
		{

			glDepthMask(GL_FALSE);

			visualS->use();

			sun->activateTexturesVisualization(visualS, GL_TEXTURE0, 0);

			//glViewport(0, 0, sun.aspect.x, sun.aspect.y);

			GLint polygonMod;
			glGetIntegerv(GL_POLYGON_MODE, &polygonMod);

			if (polygonMod != GL_FILL)
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			glBindVertexArray(smVao);
			glDrawElements(GL_TRIANGLES, smInd.size() * 3, GL_UNSIGNED_INT, 0);

			glPolygonMode(GL_FRONT_AND_BACK, polygonMod);

			glDepthMask(GL_TRUE);

		}

		if (show_ct)
		{
			glDepthMask(GL_FALSE);
			glDisable(GL_DEPTH_TEST);

			vTestCompS->use();
			glActiveTexture(GL_TEXTURE0);
			//mainCamera->bindTextureDepth();
			mainCamera->bindTextureValid();
			//glBindTexture(GL_TEXTURE_2D, reduceImg[reduceImg.size() - 1]);
			//glBindTexture(GL_TEXTURE_2D, fitBoundingBox[fitBoundingBox.size() - 1][1]);
			//glBindTexture(GL_TEXTURE_2D, reduceImg[2]);
			//glBindTexture(GL_TEXTURE_2D, testReduce);
			vTestCompS->setInt("imgTest", 0);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, reduceImg2[reduceImg2.size() - 1]);
			vTestCompS->setInt("imgTest2", 1);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, reduceImgMinMaxOnes[reduceImgMinMaxOnes.size() - 1]);
			vTestCompS->setInt("imgTestMinMaxOnes", 2);
			vTestCompS->setInt("alg", static_cast<int>(cascAlg));
			vTestCompS->setFloat("near", mainCamera->getZNear());
			vTestCompS->setFloat("far", mainCamera->getZFar());
			vTestCompS->setBool("test", show_test_reduce);
			vTestCompS->setFloat("height", (mainCamera->getPos().z) / (mainCamera->getZFar() - mainCamera->getZNear()));
			vTestCompS->setVec2("aspect", glm::value_ptr(glm::vec2(testReduceAspect)));
			vTestCompS->setBool("picture_test", use_picture_test);
			//glViewport(0, 0, sun.aspect.x, sun.aspect.y);

			GLint polygonMod;
			glGetIntegerv(GL_POLYGON_MODE, &polygonMod);

			if (polygonMod != GL_FILL)
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			glBindVertexArray(smVao);
			glDrawElements(GL_TRIANGLES, smInd.size() * 3, GL_UNSIGNED_INT, 0);

			glPolygonMode(GL_FRONT_AND_BACK, polygonMod);

			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
		}
	}

	if (show_fps)
		renderFPScounter();

	menuController->renderMenu();

	glutSwapBuffers();
	glutPostRedisplay();
}

void CascadeRenderer::loadSaveFile(int i)
{
	std::cout << "state loaded from savestates/save" << i << ".txt\n";

	std::ifstream file;
	std::string path = "savestates/save" + std::to_string(i) + ".txt";
	file.open(path);
	if (file.is_open())
	{
		for (int j = 0; j < 5; j++)
		{
			std::string line;
			std::getline(file, line);
			std::stringstream lss(line);
			std::string s;
			std::vector<std::string> seg;
			while (std::getline(lss, s, ','))
			{
				seg.push_back(s);
			}

			switch (j)
			{
				case 0:
					camPosition = glm::vec3(std::stof(seg[0]), std::stof(seg[1]), std::stof(seg[2]));
					break;

				case 1:
					if (menuController->getMenuActive())
					{
						menuController->setMouseXY(std::stoi(seg[0]), std::stoi(seg[1]));
					}
					else
					{
						glutWarpPointer(std::stoi(seg[0]), std::stoi(seg[1]));
					}
					break;

				case 2:
					alpha = std::stof(seg[0]);
					break;

				default:
					break;
			}

			lss.clear();
			seg.clear();
		}

		file.close();
	}
	else
	{
		std::cout << "unable to open savefile" << i << ".txt\n";
	}
}

void CascadeRenderer::saveSaveFile(int i)
{
	std::cout << "state saved in savestates/save" << i << ".txt\n";

	std::ofstream file;
	std::string path = "savestates/save" + std::to_string(i) + ".txt";
	file.open(path);
	if (file.is_open())
	{
		std::string cp = std::to_string(camPosition.x) + "," + std::to_string(camPosition.y) + "," + std::to_string(camPosition.z);
		std::string mp;
		if (menuController->getMenuActive())
		{
			int mx, my;
			menuController->getMouseXY(&mx, &my);
			mp = std::to_string(mx) + "," + std::to_string(my);
		}
		else
		{
			mp = std::to_string(mouseX) + "," + std::to_string(mouseY);
		}
		std::string sa = std::to_string(alpha);
		file << cp << "\n";
		file << mp << "\n";
		file << sa;
		file.close();
	}
	else
	{
		std::cout << "unable to open savefile" << i << ".txt\n";
	}
}

void CascadeRenderer::exportScreenshot(bool linear)
{
	std::vector<float> data;
	std::vector<float> data2;
	std::vector<uint8_t> dataValid;
	std::vector<uint8_t> dataValid2;
	data.resize(mainCamera->getAspect().x * mainCamera->getAspect().y);
	data2.resize(mainCamera->getAspect().x * mainCamera->getAspect().y * 3);
	dataValid.resize(mainCamera->getAspect().x * mainCamera->getAspect().y * 3);
	dataValid2.resize(mainCamera->getAspect().x * mainCamera->getAspect().y * 3);
	glActiveTexture(GL_TEXTURE0);
	mainCamera->bindTextureDepth();
	glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data.data());

	glActiveTexture(GL_TEXTURE0);
	mainCamera->bindTextureValid();
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, dataValid.data());

	if (!linear)
	{

		for (int x = 0; x < mainCamera->getAspect().x; ++x)
		{
			for (int y = 0; y < mainCamera->getAspect().y; ++y)
			{
				float depth = data[x + y * mainCamera->getAspect().x];
				int idx = (x + (mainCamera->getAspect().y - y - 1) * mainCamera->getAspect().x) * 3;
				data2[idx] = depth;
				data2[idx + 1] = depth;
				data2[idx + 2] = depth;
			}
		}

		for (int x = 0; x < mainCamera->getAspect().x; ++x)
		{
			for (int y = 0; y < mainCamera->getAspect().y; ++y)
			{
				for (int i = 0; i < 3; i++)
					dataValid2[(mainCamera->getAspect().x * y + x) * 3 + i] = dataValid[(mainCamera->getAspect().x * (mainCamera->getAspect().y - y - 1) + x) * 3 + i];
			}
		}

		std::string path = "screenshots/depth";
		path += std::to_string(screenshots_unlinearized_taken++);

		system(("mkdir \"" + path + "\"").c_str());

		stbi_write_hdr((path + "/depth.hdr").c_str(), mainCamera->getAspect().x, mainCamera->getAspect().y, 3, data2.data());

		stbi_write_png((path + "/valid.png").c_str(), mainCamera->getAspect().x, mainCamera->getAspect().y, 3, dataValid2.data(), mainCamera->getAspect().x * 3);

		std::ofstream file;
		file.open(path + "/height.txt");
		if (file.is_open())
		{
			std::string sa = std::to_string((mainCamera->getPos().z) / (mainCamera->getZFar() - mainCamera->getZNear()));
			file << sa;
			file.close();
		}
		else
		{
			std::cout << "unable to open " + path + "/height.txt\n";
		}
		file.close();
	}
	else
	{
		for (int x = 0; x < mainCamera->getAspect().x; ++x)
		{
			for (int y = 0; y < mainCamera->getAspect().y; ++y)
			{
				float depth = mainCamera->LinearizeDepth(data[x + y * mainCamera->getAspect().x]);
				int idx = (x + (mainCamera->getAspect().y - y - 1) * mainCamera->getAspect().x) * 3;
				data2[idx] = depth;
				data2[idx + 1] = depth;
				data2[idx + 2] = depth;
			}
		}

		for (int x = 0; x < mainCamera->getAspect().x; ++x)
		{
			for (int y = 0; y < mainCamera->getAspect().y; ++y)
			{
				for (int i = 0; i < 3; i++)
					dataValid2[(mainCamera->getAspect().x * y + x) * 3 + i] = dataValid[(mainCamera->getAspect().x * (mainCamera->getAspect().y - y - 1) + x) * 3 + i];
			}
		}

		std::string path = "screenshots/depth";
		path += std::to_string(screenshots_linearized_taken++);
		path += "_linear";

		system(("mkdir \"" + path + "\"").c_str());

		stbi_write_hdr((path + "/depth.hdr").c_str(), mainCamera->getAspect().x, mainCamera->getAspect().y, 3, data2.data());

		stbi_write_png((path + "/valid.png").c_str(), mainCamera->getAspect().x, mainCamera->getAspect().y, 3, dataValid2.data(), mainCamera->getAspect().x * 3);

		std::ofstream file;
		file.open(path + "/height.txt");
		if (file.is_open())
		{
			std::string sa = std::to_string((mainCamera->getPos().z) / (mainCamera->getZFar() - mainCamera->getZNear()));
			file << sa;
			file.close();
		}
		else
		{
			std::cout << "unable to open " + path + "/height.txt\n";
		}
		file.close();
	}
	data.clear();
	data2.clear();
	dataValid.clear();
	dataValid2.clear();
}

void CascadeRenderer::makeScreenshot()
{
	std::vector<uint8_t> data;
	std::vector<uint8_t> data2;
	data.resize(windowHeight * windowWidth * 4);
	data2.resize(windowHeight * windowWidth * 4);
	glReadPixels(0, 0, windowWidth, windowHeight, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

	for (int x = 0; x < windowWidth; ++x)
	{
		for (int y = 0; y < windowHeight; ++y)
		{
			for (int i = 0; i < 4; i++)
				data2[(windowWidth * y + x) * 4 + i] = data[(windowWidth * (windowHeight - y - 1) + x) * 4 + i];
		}
	}

	stbi_write_png(("screenshots/sc" + std::to_string(screenshots_taken++) + ".png").c_str(), windowWidth, windowHeight, 4, data2.data(), windowWidth * 4);
	data.clear();
	data2.clear();
}

void CascadeRenderer::input()
{
	if (!KeyState('\t') && !KeyState('o') && KeyState('i') && !swap_ls)
	{
		for (int i = 1; i < 10; i++)
		{
			if (KeyState(std::to_string(i).c_str()[0]))
			{
				swap_ls = true;
				loadSaveFile(i);
			}
		}
	}

	if (menuController->getMenuActive())
	{
		menuController->getMouseXY(&mouseX, &mouseY);
	}
	else
	{
		MouseXY(&mouseX, &mouseY);
	}

	float lat = ((double)mouseY / (float)windowHeight - 0.5f) * (float)M_PI * 0.99f;
	float lon = (float)M_PI * 2.0f * (1.0f - ((float)(mouseX - 11) / (float)(windowWidth - 22)));
	glm::vec2 dir = glm::vec2(-cos(lon), -sin(lon));

	if (KeyState('w'))
	{
		camPosition.x += speed * dir.x;
		camPosition.y += speed * dir.y;
	}

	if (KeyState('s'))
	{
		camPosition.x -= speed * dir.x;
		camPosition.y -= speed * dir.y;
	}
	if (KeyState('a'))
	{
		camPosition.x -= speed * dir.y;
		camPosition.y += speed * dir.x;
	}
	if (KeyState('d'))
	{
		camPosition.x += speed * dir.y;
		camPosition.y -= speed * dir.x;
	}
	if (KeyState(' '))
	{
		camPosition.z += speed;
	}
	if (KeyState('c'))
	{
		camPosition.z -= speed;
	}

	if (!swap_sm && KeyState('f'))
	{
		show_sm = !show_sm;
		swap_sm = true;
	}
	if (swap_sm && !KeyState('f'))
	{
		swap_sm = false;
	}

	if (!swap_ct && KeyState('z'))
	{
		show_ct = !show_ct;
		swap_ct = true;
	}
	if (swap_ct && !KeyState('z'))
	{
		swap_ct = false;
	}

	if (!swap_fr && KeyState('n'))
	{
		show_fr = !show_fr;
		swap_fr = true;
	}
	if (swap_fr && !KeyState('n'))
	{
		swap_fr = false;
	}

	if (!swap_casc && KeyState('j'))
	{
		show_casc = !show_casc;
		swap_casc = true;
	}
	if (swap_casc && !KeyState('j'))
	{
		swap_casc = false;
	}

	if (!swap_sun && KeyState('p'))
	{
		move_sun = !move_sun;
		swap_sun = true;
	}
	if (swap_sun && !KeyState('p'))
	{
		swap_sun = false;
	}

	if (!swap_pause && KeyState('b'))
	{
		pause_calc = !pause_calc;
		if (pause_calc)
		{
			for (Camera* c : dummys)
			{
				c->~Camera();
				free(c);
			}
			dummys.clear();
			for (Camera* c : mainCamera->splitAt(sun->getSplits()))
			{
				c->initialize();
				dummys.push_back(c);
			}

		}
		swap_pause = true;
	}
	if (swap_pause && !KeyState('b'))
	{
		swap_pause = false;
	}

	if (!swap_test_reduce && KeyState('v'))
	{
		show_test_reduce = !show_test_reduce;
		swap_test_reduce = true;
	}
	if (swap_test_reduce && !KeyState('v'))
	{
		swap_test_reduce = false;
	}

	if (!swap_bbfitting && KeyState('x'))
	{
		bbfitting_active = !bbfitting_active;
		swap_bbfitting = true;
	}
	if (swap_bbfitting && !KeyState('x'))
	{
		swap_bbfitting = false;
	}

	if (!swap_screenshot && KeyState('y'))
	{
		exportScreenshot(KeyState('\t'));
		swap_screenshot = true;
	}
	if (swap_screenshot && !KeyState('y'))
	{
		swap_screenshot = false;
	}

	if (!swap_screenshot_screen && KeyState('q'))
	{
		makeScreenshot();
		swap_screenshot_screen = true;
	}
	if (swap_screenshot_screen && !KeyState('q'))
	{
		swap_screenshot_screen = false;
	}

	if (!KeyState('\t') && !KeyState('i') && !KeyState('o'))
	{
		if (KeyState('1'))
		{
			cascAlg = CascadeAlgorithm::FIXED_SPLITS;
		}
		if (KeyState('2'))
		{
			cascAlg = CascadeAlgorithm::MIN_MAX_SDSM;
		}
		if (KeyState('3'))
		{
			cascAlg = CascadeAlgorithm::MOMENT_BASED_SDSM;
		}
	}

	camTarget = glm::vec3(cos(lat) * cos(lon), cos(lat) * sin(lon), sin(lat));

	if (!menuController->getMenuActive() && !cs)
	{
		RECT windowRect;
		HWND hwnd = GetActiveWindow();
		GetWindowRect(hwnd, &windowRect);
		windowRect.bottom -= 17;
		windowRect.top += 41;
		windowRect.left += 9;
		windowRect.right -= 9;
		while (ClipCursor(&windowRect) == 0);
		cs = true;
	}
	if (menuController->getMenuActive())
	{
		if (cs)
		{
			while (ClipCursor(NULL) == 0);
		}
		cs = false;
	}
	if (mouseX >= windowWidth - 10) glutWarpPointer(11, mouseY);
	if (mouseX <= 10) glutWarpPointer(windowWidth - 11, mouseY);

	if (!KeyState('\t') && !KeyState('i') && KeyState('o') && !swap_ls)
	{
		for (int i = 1; i < 10; i++)
		{
			if (KeyState(std::to_string(i).c_str()[0]))
			{
				swap_ls = true;
				saveSaveFile(i);
			}
		}
	}
	if (!KeyState('i') && !KeyState('o'))
	{
		swap_ls = false;
	}
}

void CascadeRenderer::timer()
{
	if (move_sun)
	{
		alpha += 0.002f * 180.0f / M_PI;
		if (360.0f <= alpha)
			alpha = 0.0f;
	}

	mainCamera->updateCameraView(camPosition, camTarget, camUp);
	if (!pause_calc)
		mainCamera->updateCameraProjection(45.0f, 0.01f, 5.0f);
	else
		mainCamera->updateCameraProjection(45.0f, 0.01f, 50.0f);

	/*
	dummyCamera->updateAspect(glm::ivec2(200, 200));
	glm::vec3 dummmyPos = glm::vec3(0.0f, 0.001f, 3.0f);
	dummyCamera->updateCameraView(dummmyPos, glm::normalize(dummmyPos), camUp);
	dummyCamera->updateCameraProjection(45.0f, 0.1f, 1.0f);
	*/

	matM = glm::mat4x4(1.0f) * glm::scale(glm::vec3(0.02f * scObjects)) * glm::rotate((float)M_PI / 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));

	matMVP = mainCamera->getMatVP() * matM;
	//matNrml = glm::transpose(glm::inverse(mainCamera->getMatV() * matM));
	matNrml = glm::transpose(glm::inverse(glm::mat3(matM)));

	if (sun->getAspect().x != sm_size)
	{
		sun->updateAspect(sm_size, sm_size);
	}

}

void CascadeRenderer::reloadShader()
{
	reduceSumCompS = new Shader("csm_reduce_sum.comp");
	reduceSumCompS->use();
	reduceSumCompSInOne = new Shader("csm_reduce_sum_one_for_all.comp");
	reduceSumCompSInOne->use();
	reduceMinMaxCompS = new Shader("csm_reduce_min_max.comp");
	reduceMinMaxCompS->use();
	reduceMinMaxOnesCompS = new Shader("csm_reduce_min_max_ones.comp");
	reduceMinMaxOnesCompS->use();
	calcSplitsFixedS = new Shader("csm_calculate_splits_fixed.comp");
	calcSplitsFixedS->use();
	calcSplitsMinMaxS = new Shader("csm_calculate_splits_min_max.comp");
	calcSplitsMinMaxS->use();
	calcSplitsMomentsS = new Shader("csm_calculate_splits_moments.comp");
	calcSplitsMomentsS->use();
	calcBoundingBoxFittingS = new Shader("csm_calculate_boundingbox_fitting.comp");
	calcBoundingBoxFittingS->use();
	createSumTestCompS = new Shader("csm_create_test_reduce_sum.comp");
	createSumTestCompS->use();
	createMinMaxTestCompS = new Shader("csm_create_test_reduce_min_max.comp");
	createMinMaxTestCompS->use();

	vTestCompS = new Shader("csm_visualize_compute_tex.vert", "csm_visualize_compute_tex.frag");
	vTestCompS->use();
	visualS = new Shader("csm_visualize_depth.vert", "csm_visualize_depth.frag");
	visualS->use();
	createShS = new Shader("csm_createsm.vert", "csm_createsm.frag");
	createShS->use();
	createDeS = new Shader("csm_createdepth.vert", "csm_createdepth.frag");
	createDeS->use();
	objS = new Shader("csm_obj.vert", "csm_obj.frag");
	objS->use();
	lightS = new Shader("csm_light.vert", "csm_light.frag");
	lightS->use();
	vfrustumCS = new Shader("csm_show_frustum_casc.vert", "csm_show_frustum_casc.frag");
	vfrustumCS->use();
	vfrustumS = new Shader("csm_show_frustum.vert", "csm_show_frustum.frag");
	vfrustumS->use();
}