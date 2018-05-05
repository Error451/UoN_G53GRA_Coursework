#include "MyAlienCube.h"
#include "InputHandler.h"
#include "SimpleObjects.h"
#include <iostream>

#define _MAX_MAX_RATE_OF_ROT 20000.0f
#define _MAX_RATE_OF_ROT 4000.0f
#define _MIN_MAX_RATE_OF_ROT 1000.0f

MyAlienCube::MyAlienCube(Shader* program, bool isTextured)
	: MyCube(program, isTextured), _rotx(0.0f), _roty(0.0f), _rotz(0.0f),
	_flagWireframe(false), _xpress(false), _xrelease(true)
{
	_rateofx = _MAX_RATE_OF_ROT * static_cast<GLfloat>(rand()) / static_cast<GLfloat>(RAND_MAX);
	_rateofy = _MAX_RATE_OF_ROT * static_cast<GLfloat>(rand()) / static_cast<GLfloat>(RAND_MAX);
	_rateofz = _MAX_RATE_OF_ROT * static_cast<GLfloat>(rand()) / static_cast<GLfloat>(RAND_MAX);

	rotationRateCopy = vec3(_rateofx, _rateofy, _rateofz);
}


MyAlienCube::~MyAlienCube()
{
}

void MyAlienCube::Display()
{
	_program->Use();

	GLuint program = _program->GetProgram();

	glm::mat4 view = Scene::GetCamera()->GetViewMatrix();
	glm::mat4 projection = Scene::GetCamera()->GetProjectionMatrix();

	Rotate(_roty, glm::vec3(0.0f, 1.0f, 0.0f));
	Rotate(_rotx, glm::vec3(1.0f, 0.0f, 0.0f));
	Rotate(_rotz, glm::vec3(0.0f, 0.0f, 1.0f));

	//set shader uniforms
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &_model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, &projection[0][0]);

	//animation variable - breath like effect
	glUniform1f(glGetUniformLocation(program, "emission"), abs(sin(Engine::_renderTime)));

	if (bTextured)
	{
		GLuint offset = 0;
		int i = 0;
		//render each face individually - uses texGeneralRenderer
		for (int i = 0; i < 12; i += 2)
		{
			glUniform1i(glGetUniformLocation(program, "material.texture_diffuse1"), 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, cubeTextures[i]);
			//glBindTexture(GL_TEXTURE_2D, MyScene::GetDefaultTexture());

			glUniform1i(glGetUniformLocation(program, "material.texture_specular1"), 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, cubeTextures[i + 1]);
			//glBindTexture(GL_TEXTURE_2D, MyScene::GetDefaultTexture());

			glUniform1f(glGetUniformLocation(program, "material.shininess"), _material.shininess);

			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, &(cubeIndices[i][0]));
			//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			glBindTexture(GL_TEXTURE_2D, 0);

		}
	}
	else
	{
		//cube without texture - uses general renderer
		glUniform3fv(glGetUniformLocation(program, "material.ambient"), 1, glm::value_ptr(_material.ambient));
		glUniform3fv(glGetUniformLocation(program, "material.diffuse"), 1, glm::value_ptr(_material.diffuse));
		glUniform3fv(glGetUniformLocation(program, "material.specular"), 1, glm::value_ptr(_material.specular));
		glUniform1f(glGetUniformLocation(program, "material.shininess"), _material.shininess);

		glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // 36 vertices to form 12 triangles for 1 cube
		glBindVertexArray(0);
	}
}

void MyAlienCube::Update(const GLfloat& deltaTime)
{
	if (InputHandler::GetKeyState(GLFW_KEY_LEFT))
	{
		if (_rateofx < _MAX_MAX_RATE_OF_ROT)
		{
			_rateofx += 100;
			_rateofy += 100;
			_rateofz += 100;

		}
	}

	if (InputHandler::GetKeyState(GLFW_KEY_RIGHT))
	{
		if (_rateofx > _MIN_MAX_RATE_OF_ROT)
		{
			_rateofx -= 100;
			_rateofy -= 100;
			_rateofz -= 100;

		}
	}

	_rotx = glm::radians(_rateofx) * deltaTime;
	_roty = glm::radians(_rateofy) * deltaTime;
	_rotz = glm::radians(_rateofz) * deltaTime;
}