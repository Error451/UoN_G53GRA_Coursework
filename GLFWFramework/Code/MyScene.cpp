#include "MyScene.h"

#include <ctime>
#include <iostream>

#include "Objects/Cube.h"
#include "Objects/MyCube.h"
#include "Objects/WarpParticles.h"
#include "Objects/CubeMap.h"
#include "Objects/Model.h"
#include "Objects/MyAlienCube.h"

#include "SimpleObjects.h"
#include "InputHandler.h"

//########################################################################################
//#Note: Framework file: Camera.cpp / Texture.cpp / Engine.cpp / Scene.cpp               #
//#is slightly modified because of some problematic functionality and adjust             #
//#with some techniques used in the program.                                             #
//#                                                                                      #
//#The original image loader SOIL library is replace by the std_image                    #
//#loader (older but effective), because the SOIL library                                #
//#did not work correctly (can only accept .png file, color format is messy as well)     #
//#                                                                                      #
//#A new model loader assimp is installed to complete the scene.                         #
//########################################################################################

//cubemap used as skybox
std::vector<std::string> cmFiles
{
	"./Textures/stormydays_rt.png",
	"./Textures/stormydays_lf.png",
	"./Textures/stormydays_up.png",
	"./Textures/stormydays_dn.png",
	"./Textures/stormydays_bk.png",
	"./Textures/stormydays_ft.png"
};

MyScene::MyScene(int argc, char** argv, const char *title, const int& windowWidth, const int& windowHeight)
	: Scene(argc, argv, title, windowWidth, windowHeight), 
	resolution(1.0f), radius(2.0f), sigma(3.0f), exposure(1.5f),isBloom(true)
{
	// don't construct/call opengl commands here - except global setting
	srand(static_cast<unsigned int>(time(nullptr)));
	scrWidth = Engine::GetWindowWidth();
	scrHeight = Engine::GetWindowHeight();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

MyScene::~MyScene() 
{
	//release memory
	glDeleteBuffers(1, &hdrFBO);
	glDeleteBuffers(1, &rboDepth);
	glDeleteBuffers(2, blurFBOs);

	glDeleteTextures(2, colorBuffers);
	glDeleteTextures(2, blurColorBuf);

	for (std::vector<MyLight*>::iterator it = mylights.begin(); it != mylights.end(); ++it)
	{
		if (*it != nullptr)
		{
			delete *it;
			*it = nullptr;
		}
	}
	mylights.clear();

	glDeleteTextures(1, &texDefault);
}

void MyScene::BloomInit()
{
	// configure (floating point) framebuffers
	// ---------------------------------------
	glGenFramebuffers(1, &hdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

	// create 2 floating point color buffers (1 for normal rendering, other for brightness treshold values
	glGenTextures(2, colorBuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, scrWidth, scrHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// attach texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
	}

	// create and attach depth buffer (renderbuffer)
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, scrWidth, scrHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "HDR Framebuffer not complete!" << std::endl;

	//unbind hdrFBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// ping-pong-framebuffer for blurring
	glGenFramebuffers(2, blurFBOs);
	glGenTextures(2, blurColorBuf);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, blurFBOs[i]);

		glBindTexture(GL_TEXTURE_2D, blurColorBuf[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, scrWidth, scrHeight, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blurColorBuf[i], 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		// also check if framebuffers are complete (no need for depth buffer)
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Blur Framebuffer not complete!" << std::endl;

		//unbind current fbo -> prevent mistake, 0-> default frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void MyScene::lightSettingInit()
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

void MyScene::LoadTextures()
{
	LoadTexture("./Textures/5dim_rt.tga", "floor");
	LoadTexture("./Textures/5dim_up.tga", "wall_ft");
	LoadTexture("./Textures/5dim_ft.tga", "wall_rt");
	LoadTexture("./Textures/Scifi_Hex_Wall_diffuse.jpg", "bk_diff");
	LoadTexture("./Textures/Scifi_Hex_Wall_specular.jpg","bk_spec");
	LoadTexture("./Textures/cyan.jpg", "side");
	LoadTexture("./Textures/container.png", "containerS");
	LoadTexture("./Textures/container_e.png", "containerE");
}

void MyScene::Initialise()
{	
	Scene::Initialise();

	//load textures
	LoadTextures();

	//print useful limits
	std::cout << "Note : Maximum draw buffers allowed : " << GL_MAX_DRAW_BUFFERS << "\n";
	std::cout << "Note : Maximum texture unit allowed : " << GL_MAX_TEXTURE_UNITS << "\n";

	//objects
	//Cube *c;
	MyCube *mc;
	MyCube *myfloor;
	MyCube *mywall1;
	MyCube *mywall2;
	GLfloat tx, ty, tz;

	//bind shaders
	texGeneralRenderer = new Shader("./Code/Shaders/texturedrenderer.vert", "./Code/Shaders/texturedrenderer.frag");
	generalRenderer = new Shader("./Code/Shaders/generalrenderer.vert", "./Code/Shaders/generalrenderer.frag");
	gaussian = new Shader("./Code/Shaders/gaussian.vert", "./Code/Shaders/gaussian.frag");
	videobloom = new Shader("./Code/Shaders/addbloom.vert", "./Code/Shaders/addbloom.frag");
	lamp = new Shader("./Code/Shaders/lamp.vert", "./Code/Shaders/lamp.frag");
	skyboxRenderer = new Shader("./Code/Shaders/skybox.vert", "./Code/Shaders/skybox.frag");
	fighterRenderer = new Shader("./Code/Shaders/texturedrenderer.vert", "./Code/Shaders/modelrenderer.frag");
	alienRenderer = new Shader("./Code/Shaders/texturedrenderer.vert", "./Code/Shaders/alienrenderer.frag");


	//add shaders for light registration
	AddShaderToLightQueue(generalRenderer);
	AddShaderToLightQueue(texGeneralRenderer);
	AddShaderToLightQueue(fighterRenderer);
	AddShaderToLightQueue(alienRenderer);

	//initialize light setting
	lightSettingInit();

	//initialize video bloom
	BloomInit();
	//initialize full screen quads
	//InitFullScreenQuad();

	skybox = new CubeMap(skyboxRenderer);
	skybox->loadCubeMap(cmFiles);

	GLuint alienEmissive, alienSpecular;
	alienEmissive = myTextures.find("containerE")->second;
	alienSpecular = myTextures.find("containerS")->second;
#if 1
	//10 alien box
	for (int i = 0; i < 10; ++i)
	{
		//mc = new MyAlienCube(generalRenderer, false);
		mc = new MyAlienCube(alienRenderer, true);

		tx = static_cast<GLfloat>(rand() % 20 - 10);
		ty = static_cast<GLfloat>(rand() % 10 - 4);
		tz = static_cast<GLfloat>(rand() % 20 - 10);
		mc->Translate(vec3(tx, ty, tz));
		mc->SetMaterial(alienEmissive, alienSpecular);

		AddObjectToScene(mc);
	}

	//10 normal box
	for (int i = 0; i < 10; ++i)
	{
		//mc = new MyAlienCube(generalRenderer, false);
		mc = new MyCube(generalRenderer, false);

		tx = static_cast<GLfloat>(rand() % 20 - 10);
		ty = static_cast<GLfloat>(rand() % 10 - 4);
		tz = static_cast<GLfloat>(rand() % 20 - 10);
		mc->Translate(vec3(tx, ty, tz));
		
		AddObjectToScene(mc);
	}

#endif

#if 1
	//set up toy box
	GLuint dTex, boxBackDiffuse, boxBackSpecular, boxSide;

	boxBackDiffuse = myTextures.find("bk_diff")->second;
	boxBackSpecular = myTextures.find("bk_spec")->second;
	boxSide = myTextures.find("side")->second;

	myfloor = new MyCube(texGeneralRenderer, true);
	myfloor->Translate(vec3(0.0f, -10.5f, 10.0f));
	myfloor->Scale(vec3(20.0f, 1.0f, 40.0f));

	myfloor->SetMaterial(boxSide, boxSide);	//set texture for all sides
	dTex = myTextures.find("floor")->second;
	myfloor->SetMaterial(CUBE_TEX_TOP, dTex, dTex); // set texture for specific side
	myfloor->SetMaterial(CUBE_TEX_BOTTOM, boxBackDiffuse, boxBackSpecular);
	AddObjectToScene(myfloor);

	//correct transformation order M = T.R.S translation multiplied first, and last applied to points.

	mywall1 = new MyCube(texGeneralRenderer, true);
	mywall1->Translate(vec3(10.5f, 0.0f, 10.0f));
	mywall1->Rotate(90, vec3(0.0f, 0.0f, 1.0f));
	mywall1->Scale(vec3(20.0f, 1.0f, 40.0f));

	mywall1->SetMaterial(boxSide, boxSide);
	dTex = myTextures.find("wall_rt")->second;
	mywall1->SetMaterial(CUBE_TEX_TOP, dTex, dTex);
	mywall1->SetMaterial(CUBE_TEX_BOTTOM, boxBackDiffuse, boxBackSpecular);
	AddObjectToScene(mywall1);


	mywall2 = new MyCube(texGeneralRenderer, true);
	mywall2->Translate(vec3(0.0f, 0.0f, -10.5f));
	mywall2->Rotate(90, vec3(1.0f, 0.0f, 0.0f));
	mywall2->Scale(vec3(20.0f, 1.0f, 20.0f));

	mywall2->SetMaterial(boxSide, boxSide);
	dTex = myTextures.find("wall_ft")->second;
	mywall2->SetMaterial(CUBE_TEX_TOP, dTex, dTex);
	mywall2->SetMaterial(CUBE_TEX_BOTTOM, boxBackDiffuse, boxBackSpecular);
	AddObjectToScene(mywall2);

#endif

#if 1
	//"./Models/fighter/Corvette-F3.obj"
	Model* plane = new Model(fighterRenderer, "./Models/fighter/Corvette-F3.obj");
	plane->Translate(vec3(0.0f, -2.0f, 20.0f));
	plane->Rotate(180, vec3(0.0f, 1.0f, 0.0f));
	plane->Scale(vec3(0.005f, 0.005f, 0.005f));
	AddObjectToScene(plane);

#endif

#if 0
	mc = new MyCube(generalRenderer);
	AddObjectToScene(mc);
#endif
	//Initialise lights
	LightsInit();

#if 1
	//Create particle system
	WarpParticles* wp;
	wp = new WarpParticles(hdrFBO);
	AddObjectToScene(wp);
#endif
}

//register lights to shader, support up to 8 lights, no type limitation.
void MyScene::LightsInit()
{
	MyLight* l = new MyLight(myLightShaders, lamp, true);
	l->SetPointLight(vec3(6.0f, 8.0f, 2.0f), vec3(1.0f, 0.045f, 0.0075f),
		vec3(0.0f), vec3(0.4f, 0.8f, 1.0f), vec3(1.0f));
	AddLightToTheScene(l);

	MyLight* l2 = new MyLight(myLightShaders, lamp, true);
	l2->SetPointLight(vec3(-10.0f, 0.0f, 3.0f), vec3(1.0f, 0.045f, 0.0075f),
		vec3(0.0f), vec3(0.5f, 0.5f, 1.0f), vec3(1.0f));
	AddLightToTheScene(l2);

	MyLight* l3 = new MyLight(myLightShaders, lamp, false);
	l3->SetDirLight(vec3(0.5f, 0.5f, -0.5f), vec3(0.1f), vec3(0.6f), vec3(0.4f));

	MyLight* l4 = new MyLight(myLightShaders, lamp, true);
	l4->SetPointLight(vec3(6.0f, -8.0f, 3.0f), vec3(1.0f, 0.045f, 0.0075f),
		vec3(0.0f), vec3(0.8f, 0.5f, 0.8f), vec3(1.0f));
	AddLightToTheScene(l4);

	//MyLight* l4 = new MyLight(myLightShaders, lamp, false);
	//l4->SetDirLight(vec3(-0.5f, 0.8f, 0.3f), vec3(0.1f), vec3(0.4f, 0.43f, 0.45f), vec3(0.4f));
}

void MyScene::Draw()
{
	//render to hdrfbo instead of default frame buffer(screen)
	glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);

	//not effective anymore, now the background will be cleared by skybox (in next line)
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//skybox is separated from normal objects, since they might have different behaviours
	skybox->Display();

	//update objects
	for (DisplayableObject* obj : _objects)
		obj->Display();

	//update light bulbs (if there is any)
	for (MyLight* l : mylights)
		l->Display();

	//unbind fbo
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (isBloom)
	{
		Bloom();

		//output blurred brightness result - debugging only (just uncomment this block and compile)
		//glBindFramebuffer(GL_FRAMEBUFFER, blurFBOs[1]);
		//glReadBuffer(GL_COLOR_ATTACHMENT0);
		//glBlitNamedFramebuffer(blurFBOs[1], 0, 0, 0, scrWidth, scrHeight, 0, 0, scrWidth, scrHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	}
	else
	{
		//directly output original image if bloom is disabled
		glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glBlitNamedFramebuffer(hdrFBO, 0, 0, 0, scrWidth, scrHeight, 0, 0, scrWidth, scrHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	}

	//unbind fbo
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MyScene::Bloom()
{
	//gaussian blur with specified sigma and blur scale
	gaussian->Use();

	//smoothing parameters
	glUniform1f(glGetUniformLocation(gaussian->GetProgram(), "resolution"), resolution);
	glUniform1f(glGetUniformLocation(gaussian->GetProgram(), "radius"), radius);
	glUniform1f(glGetUniformLocation(gaussian->GetProgram(), "sigma"), sigma);

	//smoothing direction (separated gaussian for efficiency)
	GLfloat h[2] = { 1.0f, 0.0f };
	GLfloat v[2] = { 0.0f, 1.0f };
	
	//horizontal pass
	glBindFramebuffer(GL_FRAMEBUFFER, blurFBOs[0]);
	glUniform2fv(glGetUniformLocation(gaussian->GetProgram(), "dir"), 1, h);
	glUniform1i(glGetUniformLocation(videobloom->GetProgram(), "texture"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBuffers[1]);
	DrawFullScreenQuad();

	//vertical pass
	glBindFramebuffer(GL_FRAMEBUFFER, blurFBOs[1]);
	glUniform2fv(glGetUniformLocation(gaussian->GetProgram(), "dir"), 1, v);
	//bind texture -> DO NOT omit active and uniform1i, don't let opengl do anything implicit!
	glUniform1i(glGetUniformLocation(videobloom->GetProgram(), "texture"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, blurColorBuf[0]);
	DrawFullScreenQuad();

	//---------------------------------------------------
	//bind default frame buffer & unbind previous buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//clear default frame buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//bind dedicate bloom shader
	videobloom->Use();
	glUniform1f(glGetUniformLocation(videobloom->GetProgram(), "exposure"), exposure);

	//pass multiple textures to shader
	glUniform1i(glGetUniformLocation(videobloom->GetProgram(), "scene"), 0);
	glUniform1i(glGetUniformLocation(videobloom->GetProgram(), "bloom"), 1);

	//original scene
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
	//bloom
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, blurColorBuf[1]);
	//draw to screen
	DrawFullScreenQuad();

	//unbind post processing shader
	generalRenderer->Use();
}	

void MyScene::Update(const GLfloat& deltaTime)
{
	_camera.Update(deltaTime);

	for (DisplayableObject* obj : _objects)
		obj->Update(deltaTime);

	//enable or disable video bloom by press o
	if (InputHandler::GetKeyState(GLFW_KEY_O))
	{
		if (_xrelease1)
		{
			_xpress1 = true;
		}
		_xrelease1 = false;
	}
	else
	{
		if (_xpress1)
		{
			_xrelease1 = true;
			isBloom = !isBloom;
		}
		_xpress1 = false;
	}
}
