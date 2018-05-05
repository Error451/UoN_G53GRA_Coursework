#include "MyFloor.h"
#include "SimpleObjects.h"
#include "Header.h"

MyFloor::MyFloor(Shader* program, bool isTextured)
	:bIsTextured(isTextured)
{
	//shader handler
	_program = program;

	//buffer generation
	glGenVertexArrays(1, &floorVAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &NBO);
	glGenBuffers(1, &TBO);

	glBindVertexArray(floorVAO);
	//vertex pos
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	//bind normal
	glBindBuffer(GL_ARRAY_BUFFER, NBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadNorms), quadNorms, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1); //layout location 1 for normals;
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	if (isTextured)
	{
		glBindBuffer(GL_ARRAY_BUFFER, TBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadTexCoords), quadTexCoords, GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

		//set default texture
		tex_floor_diffuse = Scene::GetDefaultTexture();
		tex_floor_specular = Scene::GetDefaultTexture();
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//solid color material property
	_material.ambient = glm::vec3(0.05f);
	_material.diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
	_material.specular = glm::vec3(0.2f);
	_material.shininess = 50;

	//init model matrix
	_model = glm::mat4(1.0);
}

MyFloor::~MyFloor()
{
	//release resources
	glDeleteVertexArrays(1, &floorVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &NBO);
	glDeleteBuffers(1, &TBO);
}

void MyFloor::Display()
{
	_program->Use();
	GLuint program = _program->GetProgram();

	glm::mat4 view = Scene::GetCamera()->GetViewMatrix();
	glm::mat4 projection = Scene::GetCamera()->GetProjectionMatrix();

	//set shader uniforms
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &_model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, &projection[0][0]);

	if (bIsTextured)
	{
		glUniform1i(glGetUniformLocation(program, "material.texture_diffuse1"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex_floor_diffuse);

		glUniform1i(glGetUniformLocation(program, "material.texture_specular1"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex_floor_specular);

		glUniform1f(glGetUniformLocation(program, "material.shininess"), _material.shininess);
	}
	else
	{
		glUniform3fv(glGetUniformLocation(program, "material.ambient"), 1, value_ptr(_material.ambient));
		glUniform3fv(glGetUniformLocation(program, "material.diffuse"), 1, value_ptr(_material.diffuse));
		glUniform3fv(glGetUniformLocation(program, "material.specular"), 1, value_ptr(_material.specular));
		glUniform1f(glGetUniformLocation(program, "material.shininess"), _material.shininess);
	}

	glBindVertexArray(floorVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void MyFloor::Display(Shader* shader)
{
	shader->Use();

	glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "model"), 1, GL_FALSE, &_model[0][0]);

	glBindVertexArray(floorVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void MyFloor::Update(const GLfloat& deltaTime)
{
	//currently do nothing
}

void MyFloor::Translate(glm::vec3 trans)
{
	_model = glm::translate(_model, trans);
}

void MyFloor::Scale(glm::vec3 scale)
{
	_model = glm::scale(_model, scale);
}

//angle in degree
void MyFloor::Rotate(float angle, glm::vec3 axis)
{
	//_model = glm::rotate(_model, angle, axis);
	_model = myRotate(_model, angle, axis);
}

//set all faces in one go
void MyFloor::SetMaterial(GLuint diffuse, GLuint specular)
{
	tex_floor_diffuse = diffuse;
	tex_floor_specular = specular;
}
