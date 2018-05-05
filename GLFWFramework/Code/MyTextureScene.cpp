#include "MyTextureScene.h"

#include <ctime>
#include <iostream>

#include "Objects/Cube.h"
#include "Objects/MyCube.h"
#include "Objects/WarpParticles.h"
#include "Objects/CubeMap.h"
#include "Objects/Model.h"
#include "Objects/MyAlienCube.h"
//#include "Objects/MyFloor.h"

#include "SimpleObjects.h"
#include "InputHandler.h"

//it's called position but only direction, since directional light is infinite far away
vec3 MyTextureScene::dirLightPosition = vec3(5.0f, 5.0f, 5.0f);
vec3 MyTextureScene::dirLgithParams = vec3(0.1f, 0.6f, 0.8f);

MyTextureScene::MyTextureScene(int argc, char** argv, const char *title, const int& windowWidth, const int& windowHeight)
	: Scene(argc, argv, title, windowWidth, windowHeight)
{
	// don't construct/call opengl commands here - except global setting
	srand(static_cast<unsigned int>(time(nullptr)));
	scrWidth = Engine::GetWindowWidth();
	scrHeight = Engine::GetWindowHeight();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


MyTextureScene::~MyTextureScene()
{
}

void MyTextureScene::lightSettingInit()
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

void MyTextureScene::LightsInit()
{
	MyLight* l = new MyLight(myLightShaders, NULL, false);
	//l->SetDirLight(vec3(0.4f, 0.8f, 0.2f), vec3(0.1f), vec3(1.0f), vec3(0.6f));
	//l->SetDirLight(dirLightPosition, vec3(0.1f), vec3(1.0f), vec3(0.6f));
}

void MyTextureScene::LoadTextures()
{
	diffuse = LoadTexture("./Textures/PBR/ScifiWall/Scifi_Hex_Wall_Albedo.jpg", "diffuse");
	normal = LoadTexture("./Textures/PBR/ScifiWall/Scifi_Hex_Wall_normal2.jpg", "normal");
	specular = LoadTexture("./Textures/PBR/ScifiWall/Scifi_Hex_Wall_specular.jpg", "specualr");

	//diffuse = LoadTexture("./Textures/PBR/BrickWall/brickwall_diffuse.jpg", "diffuse");
	//normal = LoadTexture("./Textures/PBR/BrickWall/brickwall_normal.jpg", "normal");
	//specular = LoadTexture("./Textures/PBR/BrickWall/brickwall_diffuse.jpg", "specualr");

	cout << "diffuse id: " << diffuse << endl;
	cout << "specular id: " << specular << endl;
	cout << "normal id: " << normal << endl;
}

void MyTextureScene::Initialise()
{
	Scene::Initialise();

	//print useful limits
	std::cout << "Note : Maximum draw buffers allowed : " << GL_MAX_DRAW_BUFFERS << "\n";
	std::cout << "Note : Maximum texture unit allowed : " << GL_MAX_TEXTURE_UNITS << "\n";

	//bind shaders
	//generalRenderer = new Shader("./Code/Shaders/generalrenderer.vert", "./Code/Shaders/generalrenderer.frag");
	textureRenderer = new Shader("./Code/Shaders/texturetestrenderer.vs.glsl", "./Code/Shaders/texturetestrenderer.fs.glsl");

	//load textures
	LoadTextures();

	//calculate tangent and bitangent for normal map
	CalTBs();

	//add shaders for light registration
	//AddShaderToLightQueue(generalRenderer);

	//initialize light setting
	//lightSettingInit();

	mf = new MyFloor(textureRenderer, true);
	mf->Translate(vec3(0.0f, 0.0f, -10.0f));
	//mf->Rotate(-90, vec3(1.0f, 0.0f, 0.0f));
	mf->Scale(vec3(20.0f));
	mf->SetMaterial(diffuse, specular);
	AddObjectToScene(mf);

	//Initialise lights
	//LightsInit();
}

TB MyTextureScene::CalTB(vec3 pos1, vec3 pos2, vec3 pos3, vec2 uv1, vec2 uv2, vec2 uv3)
{
	vec3 edge1 = pos2 - pos1;
	vec3 edge2 = pos3 - pos1;

	vec2 deltaUV1 = uv2 - uv1;
	vec2 deltaUV2 = uv3 - uv1;

	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	TB tb;
	vec3 tangent, bitangent;
	tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	tangent = glm::normalize(tangent);

	bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
	bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
	bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
	bitangent = glm::normalize(bitangent);

	tb.tangent = tangent;
	tb.bitangent = bitangent;

	return tb;
}

void MyTextureScene::CalTBs()
{
	//just a demostration of how to calculate tangent and bitangent

	//position for 4 vertices
	vec3 pos1 = vec3(quadVerts[0][1], quadVerts[0][2], quadVerts[0][3]);
	vec3 pos2 = vec3(quadVerts[1][1], quadVerts[1][2], quadVerts[1][3]);
	vec3 pos3 = vec3(quadVerts[2][1], quadVerts[2][2], quadVerts[2][3]);
	vec3 pos4 = vec3(quadVerts[3][1], quadVerts[3][2], quadVerts[3][3]);

	//texture coordinates
	vec2 uv1 = vec2(quadTexCoords[0][1], quadTexCoords[0][2]);
	vec2 uv2 = vec2(quadTexCoords[1][1], quadTexCoords[1][2]);
	vec2 uv3 = vec2(quadTexCoords[2][1], quadTexCoords[2][2]);
	vec2 uv4 = vec2(quadTexCoords[3][1], quadTexCoords[3][2]);

	tbs.push_back(CalTB(pos1, pos2, pos3, uv1, uv2, uv3));
	tbs.push_back(CalTB(pos2, pos3, pos4, uv2, uv3, uv4));

	cout << "tangent1 : (" << tbs[0].tangent.x << ", " << tbs[0].tangent.y << ", " << tbs[0].tangent.z << ") " << endl;
	cout << "bitangent1 : (" << tbs[0].bitangent.x << ", " << tbs[0].bitangent.y << ", " << tbs[0].bitangent.z << ") " << endl;

	cout << "tangent2 : (" << tbs[1].tangent.x << ", " << tbs[1].tangent.y << ", " << tbs[1].tangent.z << ") " << endl;
	cout << "bitangent1 : (" << tbs[1].bitangent.x << ", " << tbs[1].bitangent.y << ", " << tbs[1].bitangent.z << ") " << endl;
}

void MyTextureScene::Draw()
{
	//pass1: depth map generation-------------------------

	glViewport(0, 0, scrWidth, scrHeight);

	glClearColor(0.2f, 0.2f, 0.22f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//bind unifrom to the shader
	textureRenderer -> Use();
	GLuint program = textureRenderer->GetProgram();
	glUniform3fv(glGetUniformLocation(program, "lightDir"), 1, &MyTextureScene::dirLightPosition[0]);
	glUniform3fv(glGetUniformLocation(program, "lightParams"), 1, &MyTextureScene::dirLgithParams[0]);

	//bind floor's textures (the only object in the scene so safe)
	glUniform1i(glGetUniformLocation(program, "material.texture_normal1"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, normal);

#if 1
	
	//update objects
	for (DisplayableObject* obj : _objects)
	{
		obj->Display();
	}

	//update light bulbs (if there is any)
	for (MyLight* l : mylights)
		l->Display();
#endif

	//swapbuffer performed by caller
}

void MyTextureScene::Update(const GLfloat& deltaTime)
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

	dirLightPosition.y = 5 * cos(glm::radians((float)angle));
	dirLightPosition.x = 5 * sin(glm::radians((float)angle));

	if (InputHandler::GetKeyState(GLFW_KEY_UP))
	{
		mf->Rotate(-2, vec3(1.0f, 0.0f, 0.0f));
	}

	if (InputHandler::GetKeyState(GLFW_KEY_DOWN))
	{
		mf->Rotate(2, vec3(1.0f, 0.0f, 0.0f));
	}

}

