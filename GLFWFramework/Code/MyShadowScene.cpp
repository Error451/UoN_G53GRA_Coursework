#include "MyShadowScene.h"

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

//######################################
//use left and right arrow to rotate
//directional light
//######################################

//it's called position but only direction, since directional light is infinite far away
vec3 dirLightPosition = vec3(0.0f, 5.0f, 5.0f);
float dirLightPosScale = 2.5f;

MyShadowScene::MyShadowScene(int argc, char** argv, const char *title, const int& windowWidth, const int& windowHeight)
	: Scene(argc, argv, title, windowWidth, windowHeight)
{
	// don't construct/call opengl commands here - except global setting
	srand(static_cast<unsigned int>(time(nullptr)));
	scrWidth = Engine::GetWindowWidth();
	scrHeight = Engine::GetWindowHeight();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


MyShadowScene::~MyShadowScene()
{
	//release resources
	glDeleteFramebuffers(1, &depthMapFBO);
	glDeleteTextures(1, &depthMap);
}

void MyShadowScene::lightSettingInit()
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

void MyShadowScene::LightsInit()
{
	MyLight* l = new MyLight(myLightShaders, NULL, false);
	//l->SetDirLight(vec3(0.4f, 0.8f, 0.2f), vec3(0.1f), vec3(1.0f), vec3(0.6f));
	l->SetDirLight(dirLightPosition, vec3(0.1f), vec3(1.0f), vec3(0.6f));
}

void MyShadowScene::LoadTextures()
{

}

void MyShadowScene::FBOInit()
{
	glGenFramebuffers(1, &depthMapFBO);

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 
		SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	//tell opengl frame buffer will not carry any color data
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MyShadowScene::Initialise()
{
	Scene::Initialise();

	//print useful limits
	std::cout << "Note : Maximum draw buffers allowed : " << GL_MAX_DRAW_BUFFERS << "\n";
	std::cout << "Note : Maximum texture unit allowed : " << GL_MAX_TEXTURE_UNITS << "\n";

	//bind shaders
	//generalRenderer = new Shader("./Code/Shaders/generalrenderer.vert", "./Code/Shaders/generalrenderer.frag");
	depthRenderer = new Shader("./Code/Shaders/depthrenderer.vs.glsl", "./Code/Shaders/depthrenderer.fs.glsl");
	depthDebugger = new Shader("./Code/Shaders/depthMapDebug.vs.glsl", "./Code/Shaders/depthMapDebug.fs.glsl");
	shadowMapper = new Shader("./Code/Shaders/shadowedrenderer.vs.glsl", "./Code/Shaders/shadowedrenderer.fs.glsl");

	//add shaders for light registration
	//AddShaderToLightQueue(generalRenderer);

	//initialize light setting
	//lightSettingInit();

	FBOInit();

	MyCube* mc;
	mc = new MyCube(shadowMapper, false);
	mc->Translate(vec3(6.0f, 4.0f, 5.0f));
	mc->Rotate(-10, vec3(0.0f, 1.0f, 0.0f));
	mc->Scale(vec3(3.0f, 8.0f, 3.0f));
	AddObjectToScene(mc);

	mc = new MyCube(shadowMapper, false);
	mc->Translate(vec3(0.0f, 6.0f, 10.0f));
	mc->Scale(vec3(2.0f, 2.0f, 2.0f));
	AddObjectToScene(mc);

	mc = new MyCube(shadowMapper, false);
	mc->Translate(vec3(-6.0f, 2.5f, 5.0f));
	mc->Rotate(10, vec3(0.0f, 1.0f, 0.0f));
	mc->Scale(vec3(5.0f, 5.0f, 5.0f));
	AddObjectToScene(mc);

	mc = new MyCube(shadowMapper, false);
	mc->Translate(vec3(3.0f, 5.0f, -4.0f));
	mc->Rotate(10, vec3(0.0f, 1.0f, 0.0f));
	mc->Scale(vec3(10.0f, 10.0f, 5.0f));
	AddObjectToScene(mc);

	MyFloor* mf;
	mf = new MyFloor(shadowMapper, false);
	mf->Translate(vec3(0.0f, 0.0f, 0.0f));
	mf->Rotate(-90, vec3(1.0f, 0.0f, 0.0f));
	mf->Scale(vec3(20.0f));
	AddObjectToScene(mf);

	//Initialise lights
	//LightsInit();

	//Initialise shadow matrix
	ShadowMatrixSetUp();
}

void MyShadowScene::ShadowMatrixSetUp()
{
	znear = 1.0f;
	zfar = 40.0f;

	//orthogonal projection
	mat4 lightPorjection = ortho(-20.0f, 20.0f, -20.0f, 20.0f, znear, zfar);
	mat4 lightView = lookAt(
		vec3(dirLightPosition.x * dirLightPosScale, 
			dirLightPosition.y * dirLightPosScale, 
			dirLightPosition.z * dirLightPosScale
			), vec3(0.0f), vec3(0.0f, 1.0f, 0.0f));

	lightSpaceMatrix = lightPorjection * lightView;
}

void MyShadowScene::RenderObjects()
{
	//update objects
	for (DisplayableObject* obj : _objects)
	{
		obj->Display();
	}

	//update light bulbs (if there is any)
	for (MyLight* l : mylights)
		l->Display();
}

//depth map generation call
void MyShadowScene::RenderObjects(Shader* shader)
{
	shader->Use();
	glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);

	//update objects
	for (DisplayableObject* obj : _objects)
	{
		obj->Display(shader);
	}

	//update light bulbs (if there is any)
	for (MyLight* l : mylights)
		l->Display(shader);
}

void MyShadowScene::Draw()
{
	//pass1: depth map generation-------------------------

	glViewport(0, 0, SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	//update light view matrix
	ShadowMatrixSetUp();
	//cull front face to eliminate peter panning issue
	//not work quite well, peter panning is not obvious in this scene for some reason
	//glCullFace(GL_FRONT);
	glDisable(GL_CULL_FACE);
	RenderObjects(depthRenderer);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	//----------------------------------------------------

	glViewport(0, 0, scrWidth, scrHeight);

	glClearColor(0.15f, 0.2f, 0.25f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if 0
	//debugging only - display depth map on the screen
	depthDebugger->Use();
	GLuint program = depthDebugger->GetProgram();
	glUniform1f(glGetUniformLocation(program, "znear"), znear);
	glUniform1f(glGetUniformLocation(program, "zfar"), zfar);

	glUniform1i(glGetUniformLocation(program, "depthMap"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	Scene::DrawFullScreenQuad();
#endif

#if 1
	//pass2: normal draw call with shadows
	//set up some global uniforms
	shadowMapper->Use();
	GLuint program = shadowMapper->GetProgram();

	glUniformMatrix4fv(glGetUniformLocation(program, "lightSpaceMatrix"), 1, GL_FALSE, &lightSpaceMatrix[0][0]);

	glUniform1i(glGetUniformLocation(program, "shadowMap"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glUniform3fv(glGetUniformLocation(program, "lightDir"), 1, &dirLightPosition[0]);

	RenderObjects();
#endif
}

void MyShadowScene::Update(const GLfloat& deltaTime)
{
	_camera.Update(deltaTime);

	for (DisplayableObject* obj : _objects)
		obj->Update(deltaTime);

	if (InputHandler::GetKeyState(GLFW_KEY_LEFT))
	{
		angle = (angle - increment) % 360;
	}

	if (InputHandler::GetKeyState(GLFW_KEY_RIGHT))
	{
		angle = (angle + increment) % 360;
	}

	dirLightPosition.z = 5 * cos(glm::radians((float)angle));
	dirLightPosition.x = 5 * sin(glm::radians((float)angle));
}

