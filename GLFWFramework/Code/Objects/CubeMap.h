#pragma once

#include <iostream>
#include <vector>
#include "DisplayableObject.h"

using namespace glm;

class CubeMap
	:DisplayableObject
{
public:
	CubeMap(Shader* renderer);
	~CubeMap();

	void Display();
	void Update(const GLfloat& deltaTime) {};

	void loadCubeMap(std::vector<std::string> faces);

private:
	GLuint cubemapId;
	Shader* cmRenderer;

	GLuint cubeVAO, cubeVBO, cubeEBO;
};

