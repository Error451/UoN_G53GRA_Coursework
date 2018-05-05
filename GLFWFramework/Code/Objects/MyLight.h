#pragma once

#include <glm/glm.hpp>
#include <iostream>
#include "DisplayableObject.h"
#include <vector>

using namespace glm;
using namespace std;

class MyLight :
	public DisplayableObject
{
public:
	MyLight(vector<Shader*> lightProg, Shader* bulbProg, bool isGenBulb);
	~MyLight();

	//light casters
	void SetDirLight(vec3 direction, vec3 amb, vec3 diff, vec3 spec);
	void SetPointLight(vec3 position, vec3 pointParams, vec3 amb, vec3 diff, vec3 spec);
	void SetSpotLight(vec3 position, vec3 direction, vec3 pointParams, vec2 spotParams, vec3 amb, vec3 diff, vec3 spec);

	//void SetAmbient(vec3 amb);
	//void SetDiffuse(vec3 diff);
	//void SetSepcular(vec3 spec);

	void setActive(bool act) { isActive = act; };
	int getId() { return id; };

	void Display();
	void Display(Shader* shader);
	void Update(const GLfloat& deltaTime);

	//maximum number of lights support
	static const int NR_LIGHTS = 8;

private:
	bool isActive;
	bool isBulb;
	int id;
	static int count;

	GLuint bulbVAO = 0;
	GLuint bulbVBO, bulbEBO;

	vector<Shader*> lightedShaders;
	Shader* bulbShader;

	vec3 bulbColor;
	mat4 bulbModelMat;

	void SetDirLightMain(GLuint prog, vec3 direction, vec3 amb, vec3 diff, vec3 spec);
	void SetPointLightMain(GLuint prog, vec3 position, vec3 pointParams, vec3 amb, vec3 diff, vec3 spec);
	void SetSpotLightMain(GLuint prog, vec3 position, vec3 direction, vec3 pointParams, vec2 spotParams, vec3 amb, vec3 diff, vec3 spec);
};

