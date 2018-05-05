#include "Scene.h"
#include "SimpleObjects.h"
#include <iostream>

Camera Scene::_camera = Camera();

GLuint Scene::fullScreenQuadVAO = 0;
GLuint Scene::texDefault = 0;

Scene::Scene(int argc, char** argv, const char* title, const int& windowWidth, const int& windowHeight)
	: Engine(argc, argv, title, windowWidth, windowHeight)
{
}

Scene::~Scene()
{
	for (DisplayableObject* obj : _objects)
	{
		if (obj != nullptr)
			delete obj;
		obj = nullptr;
	}
}

void Scene::Initialise()
{
	//general settings
	InitFullScreenQuad();

	//load default texture
	texHandler = Texture();
	texDefault = texHandler.GetTexture("./Textures/grey.jpg");
}

void Scene::Draw()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (DisplayableObject* obj : _objects)
		obj->Display();
}

void Scene::Update(const GLfloat& deltaTime)
{
	_camera.Update(deltaTime);

	for (DisplayableObject* obj : _objects)
		obj->Update(deltaTime);
}

void Scene::AddObjectToScene(DisplayableObject* obj)
{
	_objects.push_back(obj);
}

Camera* Scene::GetCamera()
{
	return &_camera;
}

void Scene::HandleKey(int key, int action, int mods)
{
	Input* input_obj = nullptr;

	for (DisplayableObject* obj : _objects)
	{
		input_obj = dynamic_cast<Input*>(obj);
		if (input_obj != nullptr)
			input_obj->HandleKey(key, action, mods);
	}
}

void Scene::InitFullScreenQuad()
{
	GLuint quadVBO, quadTBO;

	glGenVertexArrays(1, &fullScreenQuadVAO);
	glGenBuffers(1, &quadVBO);
	glGenBuffers(1, &quadTBO);

	glBindVertexArray(fullScreenQuadVAO);

	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0); //layout location 0 -> position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	glBindBuffer(GL_ARRAY_BUFFER, quadTBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadTexCoords), quadTexCoords, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1); //layout location 1 -> texture coordinates
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

	//unbind every thing
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Scene::DrawFullScreenQuad()
{
	//normal drawing operation
	//requires user to set up shaders - e.g. warprenderer.vert
	glBindVertexArray(fullScreenQuadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

GLuint Scene::LoadTexture(std::string path, std::string tag)
{
	GLuint tex = texHandler.GetTexture(path);
	AddTextureToTheScene(tag, tex);

	return tex;
}