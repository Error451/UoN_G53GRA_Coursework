#pragma once
#pragma once

#include "MyCube.h"

class MyAlienCube :
	public MyCube
{
public:
	MyAlienCube(Shader* program, bool isTextured);
	~MyAlienCube();

	void Update(const GLfloat& deltaTime);
	void Display();

private:
	GLboolean _flagWireframe, _xpress, _xrelease;

	GLfloat _rotx, _roty, _rotz;
	GLfloat _rateofx, _rateofy, _rateofz;

	glm::vec3 rotationRateCopy;
};

