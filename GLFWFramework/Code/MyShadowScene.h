#pragma once

#include "Scene.h"
#include "MyLight.h"
#include <string>
#include <vector>

#include "MyLight.h"

#define FBO_SIZE 512
#define NR_LIGHTS 8
#define SHADOWMAP_WIDTH 2048
#define SHADOWMAP_HEIGHT 2048

using namespace glm;
using namespace std;

class MyShadowScene
	:public Scene
{
public:
	MyShadowScene(int argc, char** argv, const char *title, const int& windowWidth, const int& windowHeight);
	~MyShadowScene();

	void Initialise();
	void Draw();	//overloaded draw
	void Update(const GLfloat& deltaTime); //overloaded update
	
private:
	void LoadTextures();
	void lightSettingInit();
	void LightsInit();
	void FBOInit();

	int scrWidth;
	int scrHeight;

	//rotation control (in degree)
	int angle = 0;
	int increment = 1;

	Shader* generalRenderer;
	Shader* shadowMapper;
	Shader* depthRenderer;
	Shader* depthDebugger;

	//depth map generation
	void RenderObjects();
	void RenderObjects(Shader* shader);
	void ShadowMatrixSetUp();
	mat4 lightSpaceMatrix;

	//orthogonal projection params
	float znear = 1.0f;
	float zfar = 7.5f;

	//lighting
	std::vector<MyLight*> mylights;
	void AddLightToTheScene(MyLight* l) { mylights.push_back(l); };

	std::vector<Shader*> myLightShaders;
	void AddShaderToLightQueue(Shader* s) { myLightShaders.push_back(s); };

	GLuint depthMap;
	//frame buffer object
	GLuint depthMapFBO;
};

