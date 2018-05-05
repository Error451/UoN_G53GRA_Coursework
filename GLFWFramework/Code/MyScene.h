#pragma once

#include "Scene.h"
#include "CubeMap.h"
#include "MyLight.h"
#include <string>

//all render texture will be 512x512
#define FBO_SIZE 512
#define NR_LIGHTS 8

using namespace glm;

class MyScene :
	public Scene
{
public:
	MyScene(int argc, char** argv, const char *title, const int& windowWidth, const int& windowHeight);
	~MyScene();
	void Draw();	//overloaded draw
	void Update(const GLfloat& deltaTime); //overloaded update

	void Initialise();

private:
	bool _xrelease1 = true;
	bool _xpress1 = false;

	Shader* gaussian;
	Shader* generalRenderer;
	Shader* videobloom;
	Shader* lamp;
	Shader* skyboxRenderer;
	Shader* texGeneralRenderer;
	Shader* fighterRenderer;
	Shader* alienRenderer;

	void lightSettingInit();

	CubeMap* skybox;

	GLuint hdrFBO;
	GLuint colorBuffers[2];
	GLuint rboDepth;
	GLuint blurFBOs[2];
	GLuint blurColorBuf[2];

	void BloomInit();
	void Bloom();

	void LightsInit();

	//shader parameters
	float resolution;
	float radius;
	float sigma;
	float exposure;

	bool isBloom;
	int scrWidth;
	int scrHeight;

	void LoadTextures();

	//lighting
	std::vector<MyLight*> mylights;
	void AddLightToTheScene(MyLight* l) { mylights.push_back(l); };

	std::vector<Shader*> myLightShaders;
	void AddShaderToLightQueue(Shader* s) { myLightShaders.push_back(s); };
	
};

