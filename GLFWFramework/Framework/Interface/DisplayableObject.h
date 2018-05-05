#pragma once

#include "Scene.h"
#include "Shader.h"

struct Material
{
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	GLfloat shininess;
};

class DisplayableObject
{
public:
	DisplayableObject(){ }
	virtual ~DisplayableObject(){}

	virtual void Display() = 0;
	//special update - use uninversial shader and do not case any specific uniform
	virtual void Display(Shader* shader) { };
	virtual void Update(const GLfloat& deltaTime) = 0;

protected:
	Shader* _program;
	Material _material;
};