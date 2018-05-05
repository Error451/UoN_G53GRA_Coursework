#include "WarpParticles.h"

#include "InputHandler.h"
#include "MyScene.h"

#include <iostream>

WarpParticles::WarpParticles(GLuint mainFrameBuffer)
	:bFirstRound(true), mainFrameBuffer(mainFrameBuffer), bFreeze(false), 
	_xpress1(false), _xrelease1(true), _xpress2(false), _xrelease2(true), _break(false)
{
	InitParams();

	//Init particle buffer------------------------------------------------------------------------------
	glGenFramebuffers(2, particleFBOs);
	glGenTextures(2, particleColorBufs);

	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, particleFBOs[i]);

		glBindTexture(GL_TEXTURE_2D, particleColorBufs[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, PTL_TEX_WIDTH, PTL_TEX_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, particleColorBufs[i], 0);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Warp Particle Framebuffer not complete !" << std::endl;

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	//--------------------------------------------------------------------------------------------------

	//compile shader
	sh_warpUpdate = new Shader("./Code/Shaders/warpupdate.vert", "./Code/Shaders/warpupdate.frag");
	sh_warpRenderer = new Shader("./Code/Shaders/warprenderer.vert", "./Code/Shaders/warprenderer.frag");

	//Init Particle Vertex------------------------------------------------------------------------------
	//note the number of vertices is 2 times of particles number to generate line (1 vertex for current position, 1 for last position)
	ptlTex = new float[PTL_2WH * 2];
	ptlVert = new float[PTL_2WH * 3];


	int Ai = 0;
	int Bi = 0;
	for (int x = 0; x < PTL_TEX_WIDTH; x++)
	{
		for (int y = 0; y < PTL_TEX_HEIGHT; y++)
		{
			ptlVert[Ai + 0] = 1;
			ptlVert[Ai + 1] = 0;
			ptlVert[Ai + 2] = 0;

			ptlVert[Ai + 3] = 0;
			ptlVert[Ai + 4] = 0;
			ptlVert[Ai + 5] = 0;

			float u = (float)x / PTL_TEX_WIDTH;
			float v = (float)y / PTL_TEX_HEIGHT;

			ptlTex[Bi + 0] = u; ptlTex[Bi + 2] = u;
			ptlTex[Bi + 1] = v; ptlTex[Bi + 3] = v;

			Ai += 6; //2 * 3, 3 = stride
			Bi += 4; //2 * 2
		}
	}

	//index array
	ptlIndices = new int[PTL_2WH];
	for (Ai = 0; Ai < PTL_2WH; Ai++) ptlIndices[Ai] = Ai;

	//generate buffers
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &TBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	//bind vertex position
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * PTL_2WH * 3, ptlVert, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
	//bind normal
	glBindBuffer(GL_ARRAY_BUFFER, TBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * PTL_2WH * 2, ptlTex, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
	//bind indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * PTL_2WH, ptlIndices, GL_STATIC_DRAW);

	//unbind everything
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//release vertex data
	delete[] ptlVert;
	delete[] ptlTex;
	delete[] ptlIndices;

	//glEnable(GL_PROGRAM_POINT_SIZE);
	//--------------------------------------------------------------------------------------------------
}

WarpParticles::~WarpParticles()
{
	glDeleteBuffers(2, particleFBOs);
	glDeleteTextures(2, particleColorBufs);
}

//helper function that normalize hex color into 0 and 1 (hexcolor is easier to search on the web)
vec4 WarpParticles::HexColor(unsigned int r, unsigned int g, unsigned int b)
{
	float fr = r / 255.0f;
	float fg = g / 255.0f;
	float fb = b / 255.0f;
	return vec4(fr,fg,fb,1.0);
}

//initialize parameters
void WarpParticles::InitParams()
{
	emitterPos = vec3(0.0f, 0.0f, 8.0f);

	emitterSize = vec3(20.0f, 20.0f, 40.0f);

	vec3 dir = vec3(0.0f, 0.0f, 1.0f);
	direction = vec4(dir, ptlSpread);

	speedParam = vec2(minSpeed, maxSpeed);

	//layout-> (throttle, life, randomseed, deltaTime)
	config = vec4(throttle, lifeTime, randomSeed, 0);

	ptlColor = HexColor(178, 226, 255);

	texelSize = vec2(1.0f / PTL_TEX_WIDTH, 1.0f / PTL_TEX_HEIGHT);

	//ratio of screen size and render texture size -> 
	//used to correct texture coordinate interpolation to [0, 1] in shader when drawing full screen quad
	texScale = vec2((float)Engine::GetWindowWidth() / PTL_TEX_WIDTH, 
		(float)Engine::GetWindowHeight() / PTL_TEX_HEIGHT);

}

void WarpParticles::PositionUpdate()
{
	//tested
	//Buffer Update---------------------------------------------------------------
	//disable color blend when updating fbos
	glDisable(GL_BLEND);
	//update delta time;
	config[3] = Engine::_renderDT;
	config[0] = throttle;
	time = (Engine::_renderTime / 20);

	sh_warpUpdate->Use();
	GLuint update = sh_warpUpdate->GetProgram();

	//shader uniforms
	speedParam = vec2(minSpeed, maxSpeed);
	glUniform3fv(glGetUniformLocation(update, "emitterPos"), 1, &emitterPos[0]);
	glUniform3fv(glGetUniformLocation(update, "emitterSize"), 1, &emitterSize[0]);
	glUniform4fv(glGetUniformLocation(update, "direction"), 1, &direction[0]);
	glUniform2fv(glGetUniformLocation(update, "speedParam"), 1, &speedParam[0]);
	glUniform4fv(glGetUniformLocation(update, "config"), 1, &config[0]);
	glUniform2fv(glGetUniformLocation(update, "texscale"), 1, &texScale[0]);

	glUniform1f(glGetUniformLocation(update, "time"), time);

	//if this is first iteration initialize textures, else update
	if (bFirstRound)
	{
		bFirstRound = false;

		//Initialization
		glBindFramebuffer(GL_FRAMEBUFFER, particleFBOs[0]);
		glUniform1i(glGetUniformLocation(update, "isInit"), 1);
		Scene::DrawFullScreenQuad();

		//initiliaze buffer switch
		bCurrentFBO = true; //1 -> second buffer
	}
	else
	{
		//debugging warpper
		//if (counter < 1)
		//{
		glBindFramebuffer(GL_FRAMEBUFFER, particleFBOs[bCurrentFBO]);
		glUniform1i(glGetUniformLocation(update, "isInit"), 0);
		glUniform1i(glGetUniformLocation(update, "particlesBuf"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, particleColorBufs[!bCurrentFBO]);

		Scene::DrawFullScreenQuad();

		bCurrentFBO = !bCurrentFBO;
		//std::cout << "aaaaaaaaaaa";
		//counter++;
		//}
	}
	//Buffer Update End-----------------------------------------------------------

	//std::cout << !bCurrentFBO;
	//visualize particle buffers - debugging
	//glBindFramebuffer(GL_FRAMEBUFFER, particleFBOs[!bCurrentFBO]);
	//glReadBuffer(GL_COLOR_ATTACHMENT0);
	//glBlitNamedFramebuffer(particleFBOs[!bCurrentFBO], mainFrameBuffer, 0, 0, 128, 128, 0, 0, 128, 128, GL_COLOR_BUFFER_BIT, GL_NEAREST);

	//resume blend
	glEnable(GL_BLEND);
}

void WarpParticles::Drawing()
{
	//Drawing particle------------------------------------------------------------
	glBindFramebuffer(GL_FRAMEBUFFER, mainFrameBuffer);


	GLuint render = sh_warpRenderer->GetProgram();
	sh_warpRenderer->Use();

	glm::mat4 view = Scene::GetCamera()->GetViewMatrix();
	glm::mat4 projection = Scene::GetCamera()->GetProjectionMatrix();

	glUniformMatrix4fv(glGetUniformLocation(render, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(render, "projection"), 1, GL_FALSE, &projection[0][0]);

	glUniform2fv(glGetUniformLocation(render, "texelSize"), 1, &texelSize[0]);

	glUniform1i(glGetUniformLocation(render, "ptlBuf1"), 0);
	glUniform1i(glGetUniformLocation(render, "ptlBuf2"), 1);

	//set up samplers
	glActiveTexture(GL_TEXTURE0);
	//bCurrentFBO->previous state, because the flag is inversed already after buffer update
	glBindTexture(GL_TEXTURE_2D, particleColorBufs[bCurrentFBO]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, particleColorBufs[!bCurrentFBO]);

	glUniform4fv(glGetUniformLocation(render, "ptlColor"), 1, &ptlColor[0]);
	glUniform1f(glGetUniformLocation(render, "ptlTail"), ptlTail); //(ptlTail / Engine::_renderDT / 60)

	glBindVertexArray(VAO);
	glDrawElements(GL_LINES, PTL_2WH, GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_POINTS, 0, PTL_2WH);
	glBindVertexArray(0);
	//MyScene::DrawFullScreenQuad();

	//debugging
	//glBindFramebuffer(GL_FRAMEBUFFER, mainFrameBuffer);
	//glReadBuffer(GL_COLOR_ATTACHMENT0);
	//glBlitNamedFramebuffer(mainFrameBuffer, 0, 0, 0, 800, 600, 0, 0, 800, 600, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	//Drawing particle end--------------------------------------------------------
}

void WarpParticles::Display()
{
	if (!bFreeze)
	{
		PositionUpdate();
	}

	Drawing();
}

void WarpParticles::Update(const GLfloat& deltaTime)
{
	//if p is pressed - freeze particle system
	if (InputHandler::GetKeyState(GLFW_KEY_P))
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

			bFreeze = !bFreeze;
			//_break = !_break;
			//std::cout << bFreeze << std::endl;
		}
		_xpress1 = false;
	}

	if(bFreeze)
	{
		return; //skip
	}

	//FTL effect
	if (InputHandler::GetKeyState(GLFW_KEY_UP))
	{
		if (_xrelease2)
		{
			_xpress2 = true;
		}
		_xrelease2 = false;

		_break = false;
		
		minSpeed = min(minSpeed + 0.1f, 15.0f);
		maxSpeed = min(maxSpeed + 0.1f, 20.0f);
		throttle = min(throttle + 0.05f, 0.5f);
		ptlTail = min(ptlTail + 1.0f, 100.0f);
	}
	else
	{
		if (_xpress2)
		{
			_xrelease2 = true;

			_break = true;
		}
		_xpress2 = false;
	}

	if (_break)
	{
		//return to initial value if 'p' is released
		minSpeed = max(minSpeed - 0.1f, 1.0f);
		maxSpeed = max(maxSpeed - 0.1f, 2.0f);
		throttle = max(throttle - 0.05f, 0.1f);
		ptlTail = max(ptlTail - 1.0f, 1.0f);
	}
}

