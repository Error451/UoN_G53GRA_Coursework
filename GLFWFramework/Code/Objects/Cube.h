#pragma once

#include "DisplayableObject.h"

class Cube :
	public DisplayableObject
{
public:
	Cube();
	Cube(Shader *program);
	~Cube();

	void Display();

	void Update(const GLfloat& deltaTime);

	void Translate(const GLfloat& tx, const GLfloat& ty, const GLfloat& tz);

	inline void SetProgram(Shader* program) { _program = program; }
protected:
	GLboolean _flagWireframe, _xpress, _xrelease;

	GLuint _VAO;

	glm::mat4 _model;

	GLfloat _rotx, _roty, _rotz;
	GLfloat _rateofx, _rateofy, _rateofz;
};

