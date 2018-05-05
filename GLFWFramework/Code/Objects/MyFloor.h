#pragma once
#include "DisplayableObject.h"

using namespace glm;

class MyFloor
	:public DisplayableObject
{
public:
	MyFloor(Shader* program, bool isTextured);
	~MyFloor();

	void Display();
	void Display(Shader* shader);
	void Update(const GLfloat& deltaTime);

	void Translate(glm::vec3 trans);
	void Scale(glm::vec3 scale);
	void Rotate(float angle, glm::vec3 axis);

	void SetMaterial(GLuint diffuse, GLuint specular);

private:
	GLuint floorVAO, VBO, NBO, TBO;
	mat4 _model;
	Shader* _program;

	//Textures
	GLuint tex_floor_diffuse;
	GLuint tex_floor_specular;

	bool bIsTextured;
};

