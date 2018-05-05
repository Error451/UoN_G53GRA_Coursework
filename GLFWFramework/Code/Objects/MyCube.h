#pragma once

#include "DisplayableObject.h"

#define CUBE_TEX_FRONT 0
#define CUBE_TEX_BACK 1
#define CUBE_TEX_LEFT 2
#define CUBE_TEX_RIGHT 3
#define CUBE_TEX_TOP 4
#define CUBE_TEX_BOTTOM 5

using namespace glm;

class MyCube :
	public DisplayableObject
{
public:
	MyCube(Shader* program, bool isTextured);
	~MyCube();

	//inherited methods
	void Display();
	void Display(Shader* shader);
	void Update(const GLfloat& deltaTime);

	void Translate(glm::vec3 trans);
	void Scale(glm::vec3 scale);
	void Rotate(float angle, glm::vec3 axis);

	void SetMaterial(GLuint diffuse, GLuint specular);
	void SetMaterial(int index, GLuint diffuse, GLuint specular);

protected:
	GLboolean _flagWireframe, _xpress, _xrelease;

	GLuint VAO, VBO, NBO, TBO, EBO;

	glm::mat4 _model;

	bool bTextured;
	std::vector<GLuint> cubeTextures;
};

