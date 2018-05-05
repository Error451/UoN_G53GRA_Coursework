#pragma once

#include "Scene.h"
#include "MyLight.h"
#include <string>
#include <vector>
#include "Objects/MyFloor.h"

#define FBO_SIZE 512
#define NR_LIGHTS 8

using namespace glm;
using namespace std;

struct TB
{
	vec3 tangent;
	vec3 bitangent;
};

class MyTextureScene
	:public Scene
{
public:
	MyTextureScene(int argc, char** argv, const char *title, const int& windowWidth, const int& windowHeight);
	~MyTextureScene();

	void Initialise();
	void Draw();	//overloaded draw
	void Update(const GLfloat& deltaTime); //overloaded update
	
private:
	void LoadTextures();
	void lightSettingInit();
	void LightsInit();

	/**TB: tangent, bitangent
	*used for transfer normal map value from tangent space (always roughly point to +z)
	*to world or view space for transformation;
	*manually calculated for quad only - for simplicity (most model loader has this functionality built-in)
	**/
	void CalTBs();
	//calculate TB for a single triangle
	TB CalTB(vec3 pos1, vec3 pos2, vec3 pos3, vec2 uv1, vec2 uv2, vec2 uv3);

	//TB lists for all triangles (only 2 actually)
	vector<TB> tbs;

	int scrWidth;
	int scrHeight;

	//rotation control (in degree)
	int angle = 0;
	int increment = 1;

	//Shaders
	Shader* generalRenderer; //my simple universal shader
	Shader* textureRenderer;

	MyFloor* mf;

	GLuint diffuse, specular, normal;

	//simple directional light position
	static vec3 dirLightPosition;
	static vec3 dirLgithParams;

	//orthogonal projection params
	float znear = 1.0f;
	float zfar = 7.5f;

	//lighting
	std::vector<MyLight*> mylights;
	void AddLightToTheScene(MyLight* l) { mylights.push_back(l); };

	std::vector<Shader*> myLightShaders;
	void AddShaderToLightQueue(Shader* s) { myLightShaders.push_back(s); };
};

