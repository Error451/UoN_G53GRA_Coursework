#pragma once

#include "DisplayableObject.h"
#include "Shader.h"

#define PTL_TEX_WIDTH 64
#define PTL_TEX_HEIGHT 64
#define PTL_2WH 8192

using namespace glm;

class WarpParticles :
	public DisplayableObject
{
public:
	WarpParticles(GLuint mainFrameBuffer);
	~WarpParticles();

	void Display();
	void Update(const GLfloat& deltaTime);

private:
	GLuint particleFBOs[2];
	GLuint particleColorBufs[2];

	GLboolean _xpress1,_xrelease1;
	GLboolean _xpress2,_xrelease2, _break;

	float* ptlVert;
	float* ptlTex;
	int* ptlIndices;

	GLuint mainFrameBuffer;

	bool bFirstRound;
	bool bFreeze;
	bool bCurrentFBO;

	//warp particle system will use its own shaders
	Shader* sh_warpUpdate;
	Shader* sh_warpRenderer;

	void InitParams();
	void PositionUpdate();
	void Drawing();
	vec4 HexColor(unsigned int r, unsigned int g, unsigned int b);

	//parameters for updating
	vec3 emitterPos;
	vec3 emitterSize;
	vec4 direction;
	vec2 speedParam;
	vec4 config;
	vec2 texScale;

	float ptlSpread = 0.01f;
	float minSpeed = 1.0f;
	float maxSpeed = 1.5f;
	float throttle = 0.1f;
	float lifeTime = 2.0f; //unit: second
	float randomSeed = 0.0f;

	vec2 texelSize;

	vec4 ptlColor;
	float ptlTail = 1.0f;

	float time;

	GLuint VAO, VBO, TBO, EBO;

	int counter = 0;
	int testProgram;
};
