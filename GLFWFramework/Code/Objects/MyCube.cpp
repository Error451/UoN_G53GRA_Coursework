#include "MyCube.h"
#include "SimpleObjects.h"
#include "InputHandler.h"
#include <iostream>
#include "MyScene.h"
#include "Header.h"

#define _MAX_RATE_OF_ROT 20.0f

MyCube::MyCube(Shader *program, bool isTextured) 
	:_flagWireframe(false), _xpress(false), _xrelease(true), bTextured(isTextured)
{
	//shader handler
	_program = program;

	//vao generation
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &NBO);
	glGenBuffers(1, &EBO);
	glGenBuffers(1, &TBO);

	//bind vao
	glBindVertexArray(VAO);
	//bind vertex position
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVerts), cubeVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0); //use layout location 0 for vertex position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	//bind normal
	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNorms), cubeNorms, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1); //layout location 1 for normals;
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	//add texture coordinates to vertex buffer if enabled
	if (bTextured)
	{
		glBindBuffer(GL_ARRAY_BUFFER, TBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTexCoords), cubeTexCoords, GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

		//init texture list
		GLuint t = Scene::GetDefaultTexture();

		cubeTextures = {t, t, t, t,
						t, t, t, t,
						t, t, t, t};
	}
	else
	{
		//bind indices - tyextured cube will bind it at run time
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
	}
	
	//glBindVertexArray(NULL);
	//unbind to avoid unexpected error;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	_material.ambient = glm::vec3(0.05f);
	_material.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	_material.specular = glm::vec3(1.0f);
	_material.shininess = 50;

	//intialize model matrix
	_model = glm::mat4(1.0);

}

MyCube::~MyCube()
{
	//release buffer
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &NBO);
	glDeleteBuffers(1, &EBO);
	glDeleteBuffers(1, &TBO);

}

void MyCube::Display()
{
	_program->Use();

	GLuint program = _program->GetProgram();

	glm::mat4 view = Scene::GetCamera()->GetViewMatrix();
	glm::mat4 projection = Scene::GetCamera()->GetProjectionMatrix();

	//set shader uniforms
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &_model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, &projection[0][0]);

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

			glUniform1i(glGetUniformLocation(program, "material.texture_specular1"), 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, cubeTextures[i + 1]);

			glUniform1f(glGetUniformLocation(program, "material.shininess"), _material.shininess);

			glBindVertexArray(VAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, &(cubeIndices[i][0]));
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

void MyCube::Display(Shader* shader)
{
	//any uniform should be handled by caller
	shader->Use();

	//model matrix
	glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "model"), 1, GL_FALSE, &_model[0][0]);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // 36 vertices to form 12 triangles for 1 cube
	glBindVertexArray(0);
}

void MyCube::Update(const GLfloat& deltaTime)
{
	//deprecated
	if (InputHandler::GetKeyState(GLFW_KEY_X))
	{
		if (_xrelease)
		{
		}
		_xrelease = false;
	}
	else
	{
		if (_xpress) _xrelease = true;
		_xpress = false;
	}
}

void MyCube::Translate(glm::vec3 trans)
{
	_model = glm::translate(_model, trans);
}

void MyCube::Scale(glm::vec3 scale)
{
	_model = glm::scale(_model, scale);
}

//angle in degree
void MyCube::Rotate(float angle, glm::vec3 axis)
{
	//_model = glm::rotate(_model, angle, axis);
	_model = myRotate(_model, angle, axis);
}

//set specific face's diffuse map and specular map
void MyCube::SetMaterial(int index, GLuint diffuse, GLuint specular)
{
	//12 texture - 2 for each side (diffuse/specular map)
	//order: front, back, left, right, top, bottom
	if (index > -1 && index < 11)
	{
		cubeTextures[2 * index] = diffuse;
		cubeTextures[2 * index + 1] = specular;
	}
}

//set all faces in one go
void MyCube::SetMaterial(GLuint diffuse, GLuint specular)
{
	for (int i = 0; i < 12; i += 2)
	{
		cubeTextures[i] = diffuse;
		cubeTextures[i + 1] = specular;
	}
}