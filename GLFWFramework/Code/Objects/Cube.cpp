#include "Cube.h"

#include "InputHandler.h"

#define _MAX_RATE_OF_ROT 20.0f

Cube::Cube() :
_rotx(0.0f), _roty(0.0f), _rotz(0.0f),
_flagWireframe(false), _xpress(false), _xrelease(true)
{
	_program = nullptr;

	GLfloat vertices[] =
	{
		//  ____coordinates____ 
		//    x      y      z  
		//        Front
		-0.5f, -0.5f, 0.5f, // 0
		0.5f, -0.5f, 0.5f,  // 1
		-0.5f, 0.5f, 0.5f, // 3
		0.5f, -0.5f, 0.5f, // 1
		0.5f, 0.5f, 0.5f, // 2
		-0.5f, 0.5f, 0.5f, // 3
		//        Back
		-0.5f, -0.5f, -0.5f, // 4
		-0.5f, 0.5f, -0.5f, // 7
		0.5f, -0.5f, -0.5f, // 5
		0.5f, 0.5f, -0.5f, // 6
		0.5f, -0.5f, -0.5f, // 5
		-0.5f, 0.5f, -0.5f, // 7
		//       Right
		0.5f, -0.5f, 0.5f, // 1
		0.5f, -0.5f, -0.5f, // 5
		0.5f, 0.5f, 0.5f, // 2
		0.5f, 0.5f, 0.5f, // 2
		0.5f, -0.5f, -0.5f, // 5
		0.5f, 0.5f, -0.5f,// 6
		//       Left
		-0.5f, -0.5f, 0.5f, // 0
		-0.5f, 0.5f, 0.5f,  // 3
		-0.5f, -0.5f, -0.5f, // 4
		-0.5f, 0.5f, 0.5f,  // 3
		-0.5f, 0.5f, -0.5f,  // 7
		-0.5f, -0.5f, -0.5f, // 4
		//		 Top
		-0.5f, 0.5f, 0.5f,  // 3
		0.5f, 0.5f, 0.5f,   // 2
		-0.5f, 0.5f, -0.5f, // 7
		0.5f, 0.5f, 0.5f,  // 2
		0.5f, 0.5f, -0.5f,   // 6
		-0.5f, 0.5f, -0.5f, // 7
		//	    Bottom
		-0.5f, -0.5f, 0.5f, // 0
		0.5f, -0.5f, -0.5f,  // 5
		0.5f, -0.5f, 0.5f, // 1
		-0.5f, -0.5f, 0.5f,  // 0
		-0.5f, -0.5f, -0.5f, // 4
		0.5f, -0.5f, -0.5f  // 5
	};

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	{
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(0);
	}
	glBindVertexArray(NULL);

	//unbind
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);

	_VAO = VAO;

	_rateofx = _MAX_RATE_OF_ROT * static_cast<GLfloat>(rand()) / static_cast<GLfloat>(RAND_MAX);
	_rateofy = _MAX_RATE_OF_ROT * static_cast<GLfloat>(rand()) / static_cast<GLfloat>(RAND_MAX);
	_rateofz = _MAX_RATE_OF_ROT * static_cast<GLfloat>(rand()) / static_cast<GLfloat>(RAND_MAX);
}


Cube::~Cube()
{
}

Cube::Cube(Shader *program) : Cube()
{
	_program = program;
}


void Cube::Display()
{
	_program->Use();

	glm::mat4 view = Scene::GetCamera()->GetViewMatrix();
	glm::mat4 projection = Scene::GetCamera()->GetProjectionMatrix();

	_model = glm::rotate(_model, _roty, glm::vec3(0.0f, 1.0f, 0.0f));
	_model = glm::rotate(_model, _rotx, glm::vec3(1.0f, 0.0f, 0.0f));
	_model = glm::rotate(_model, _rotz, glm::vec3(0.0f, 0.0f, 1.0f));

	GLuint modelLoc = glGetUniformLocation(_program->GetProgram(), "model");
	GLuint viewLoc = glGetUniformLocation(_program->GetProgram(), "view");
	GLuint prjLoc = glGetUniformLocation(_program->GetProgram(), "projection");
	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(_model));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(prjLoc, 1, GL_FALSE, glm::value_ptr(projection));

	_flagWireframe ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glBindVertexArray(_VAO);
	{
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	glBindVertexArray(0);
}

void Cube::Update(const GLfloat& deltaTime)
{
	_rotx = glm::radians(_rateofx) * deltaTime;
	_roty = glm::radians(_rateofy) * deltaTime;
	_rotz = glm::radians(_rateofz) * deltaTime;

	if (InputHandler::GetKeyState(GLFW_KEY_X))
	{
		if (_xrelease)
		{
			_xpress = true;
			_flagWireframe = !_flagWireframe;
		}
		_xrelease = false;
	}
	else
	{
		if (_xpress) _xrelease = true;
		_xpress = false;
	}

}

void Cube::Translate(const GLfloat& tx, const GLfloat& ty, const GLfloat& tz)
{
	_model = glm::translate(_model, glm::vec3(tx, ty, tz));
}