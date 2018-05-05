#include "MyAdvanceLightScene.h"

#include <ctime>
#include <iostream>

#include "Objects/Cube.h"
#include "Objects/MyCube.h"
#include "Objects/WarpParticles.h"
#include "Objects/CubeMap.h"
#include "Objects/Model.h"
#include "Objects/MyAlienCube.h"
#include "Objects/MyFloor.h"

#include "SimpleObjects.h"
#include "InputHandler.h"



MyAdvanceLightScene::MyAdvanceLightScene(int argc, char** argv, const char *title, const int& windowWidth, const int& windowHeight)
	: Scene(argc, argv, title, windowWidth, windowHeight)
{
	// don't construct/call opengl commands here - except global setting
	srand(static_cast<unsigned int>(time(nullptr)));
	scrWidth = Engine::GetWindowWidth();
	scrHeight = Engine::GetWindowHeight();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


MyAdvanceLightScene::~MyAdvanceLightScene()
{
}

void MyAdvanceLightScene::lightSettingInit()
{
	GLuint program = generalRenderer->GetProgram();

	std::string number;

	generalRenderer->Use();
	//disable all lights by default;
	for (int i = 0; i < NR_LIGHTS; i++)
	{
		number = std::to_string(i);

		glUniform1i(glGetUniformLocation(program, ("lights[" + number + "].active").c_str()), 0);
	}
}

void MyAdvanceLightScene::LightsInit()
{
	MyLight* l = new MyLight(myLightShaders, NULL, false);
	//l->SetDirLight(vec3(0.4f, 0.8f, 0.2f), vec3(0.1f), vec3(1.0f), vec3(0.6f));
	l->SetDirLight(vec3(0.4f, 0.8f, 0.2f), vec3(0.1f), vec3(1.0f), vec3(0.6f));
}

void MyAdvanceLightScene::LoadTextures()
{

}

void MyAdvanceLightScene::Initialise()
{
	Scene::Initialise();

	//print useful limits
	std::cout << "Note : Maximum draw buffers allowed : " << GL_MAX_DRAW_BUFFERS << "\n";
	std::cout << "Note : Maximum texture unit allowed : " << GL_MAX_TEXTURE_UNITS << "\n";

	//bind shaders
	generalRenderer = new Shader("./Code/Shaders/generalrenderer.vert", "./Code/Shaders/generalrenderer.frag");

	//add shaders for light registration
	AddShaderToLightQueue(generalRenderer);

	//initialize light setting
	lightSettingInit();

	MyCube* mc;
	mc = new MyCube(generalRenderer, false);
	mc->Translate(vec3(6.0f, 4.0f, 5.0f));
	mc->Rotate(-10, vec3(0.0f, 1.0f, 0.0f));
	mc->Scale(vec3(3.0f, 8.0f, 3.0f));
	AddObjectToScene(mc);

	mc = new MyCube(generalRenderer, false);
	mc->Translate(vec3(0.0f, 6.0f, 10.0f));
	mc->Scale(vec3(2.0f, 2.0f, 2.0f));
	AddObjectToScene(mc);

	mc = new MyCube(generalRenderer, false);
	mc->Translate(vec3(-6.0f, 2.5f, 5.0f));
	mc->Rotate(10, vec3(0.0f, 1.0f, 0.0f));
	mc->Scale(vec3(5.0f, 5.0f, 5.0f));
	AddObjectToScene(mc);

	MyFloor* mf;
	mf = new MyFloor(generalRenderer, false);
	mf->Translate(vec3(0.0f, 0.0f, 0.0f));
	mf->Rotate(-90, vec3(1.0f, 0.0f, 0.0f));
	mf->Scale(vec3(20.0f));
	AddObjectToScene(mf);

	//Initialise lights
	LightsInit();
}

void MyAdvanceLightScene::Draw()
{
	glClearColor(0.15f, 0.2f, 0.25f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//update objects
	for (DisplayableObject* obj : _objects)
	{
		obj->Display();
	}

	//update light bulbs (if there is any)
	for (MyLight* l : mylights)
		l->Display();
}

void MyAdvanceLightScene::Update(const GLfloat& deltaTime)
{
	_camera.Update(deltaTime);

	for (DisplayableObject* obj : _objects)
		obj->Update(deltaTime);

}

