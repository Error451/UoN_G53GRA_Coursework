#pragma once

#include "Scene.h"
#include "MyLight.h"
#include <string>
#include <vector>

#include "MyLight.h"

#define FBO_SIZE 512
#define NR_LIGHTS 8

using namespace glm;
using namespace std;

class MyAdvanceLightScene
	:public Scene
{
public:
	MyAdvanceLightScene(int argc, char** argv, const char *title, const int& windowWidth, const int& windowHeight);
	~MyAdvanceLightScene();

	void Initialise();
	void Draw();	//overloaded draw
	void Update(const GLfloat& deltaTime); //overloaded update
	
private:
	void LoadTextures();
	void lightSettingInit();
	void LightsInit();

	int scrWidth;
	int scrHeight;

	Shader* generalRenderer;

	//lighting
	std::vector<MyLight*> mylights;
	void AddLightToTheScene(MyLight* l) { mylights.push_back(l); };

	std::vector<Shader*> myLightShaders;
	void AddShaderToLightQueue(Shader* s) { myLightShaders.push_back(s); };
};

