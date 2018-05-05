#include "MyLight.h"
#include "SimpleObjects.h"
#include <string>

int MyLight::count = 0;

MyLight::MyLight(vector<Shader*> lightProg, Shader* bulbProg, bool isGenBulb)
	:lightedShaders(lightProg), bulbShader(bulbProg)
{
	if (count < NR_LIGHTS)
	{
		id = count;
		count++;
	}
	else
	{
		std::cout << "ERROR::LIGHT NUMBER EXCEED LIMITAION" << std::endl;
		exit(false);
	}

	//if requires bulb, create light bulb mesh (cube)
	if (isGenBulb)
	{
		isBulb = true;

		glGenVertexArrays(1, &bulbVAO);
		glGenBuffers(1, &bulbVBO);
		glGenBuffers(1, &bulbEBO);

		glBindVertexArray(bulbVAO);

		glBindBuffer(GL_ARRAY_BUFFER, bulbVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(simpleCubeVerts), simpleCubeVerts, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bulbEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(simpleCubeIndices), simpleCubeIndices, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	else
	{
		isBulb = false;
	}

	bulbModelMat = mat4(1.0f);
}

MyLight::~MyLight()
{
	glDeleteVertexArrays(1, &bulbVAO);
	glDeleteBuffers(1, &bulbVBO);
	glDeleteBuffers(1, &bulbEBO);
}

void MyLight::SetDirLightMain(GLuint prog, vec3 direction, vec3 amb, vec3 diff, vec3 spec)
{
	std::string number = std::to_string(id);
	glUniform1i(glGetUniformLocation(prog, ("lights[" + number + "].active").c_str()), 1);
	glUniform1i(glGetUniformLocation(prog, ("lights[" + number + "].type").c_str()), 0);

	glUniform3fv(glGetUniformLocation(prog, ("lights[" + number + "].direction").c_str()), 1, &direction[0]);
	glUniform3fv(glGetUniformLocation(prog, ("lights[" + number + "].ambient").c_str()), 1, &amb[0]);
	glUniform3fv(glGetUniformLocation(prog, ("lights[" + number + "].diffuse").c_str()), 1, &diff[0]);
	glUniform3fv(glGetUniformLocation(prog, ("lights[" + number + "].specular").c_str()), 1, &spec[0]);
}

void MyLight::SetDirLight(vec3 direction, vec3 amb, vec3 diff, vec3 spec)
{
	//local variable
	GLuint program;

	//register lights to all shaders that needs lighting
	for (unsigned int i = 0; i < lightedShaders.size(); i++)
	{
		program = lightedShaders[i]->GetProgram();
		lightedShaders[i]->Use();
		SetDirLightMain(program, direction, amb, diff, spec);
	}

	//set up bulb's information
	bulbColor = diff;
	bulbModelMat = translate(bulbModelMat, direction);
	bulbModelMat = scale(bulbModelMat, vec3(0.5f, 0.5f, 0.5f));
}

void MyLight::SetPointLightMain(GLuint prog, vec3 position, vec3 pointParams, vec3 amb, vec3 diff, vec3 spec)
{
	std::string number = std::to_string(id);

	glUniform1i(glGetUniformLocation(prog, ("lights[" + number + "].active").c_str()), 1);
	glUniform1i(glGetUniformLocation(prog, ("lights[" + number + "].type").c_str()), 1);

	glUniform1f(glGetUniformLocation(prog, ("lights[" + number + "].constant").c_str()), pointParams.x);
	glUniform1f(glGetUniformLocation(prog, ("lights[" + number + "].linear").c_str()), pointParams.y);
	glUniform1f(glGetUniformLocation(prog, ("lights[" + number + "].quadratic").c_str()), pointParams.z);

	glUniform3fv(glGetUniformLocation(prog, ("lights[" + number + "].position").c_str()), 1, &position[0]);

	glUniform3fv(glGetUniformLocation(prog, ("lights[" + number + "].ambient").c_str()), 1, &amb[0]);
	glUniform3fv(glGetUniformLocation(prog, ("lights[" + number + "].diffuse").c_str()), 1, &diff[0]);
	glUniform3fv(glGetUniformLocation(prog, ("lights[" + number + "].specular").c_str()), 1, &spec[0]);
}

void MyLight::SetPointLight(vec3 position, vec3 pointParams, vec3 amb, vec3 diff, vec3 spec)
{
	//register with default opaque renderer (the one without texturing)
	GLuint program;

	for (unsigned int i = 0; i < lightedShaders.size(); i++)
	{
		program = lightedShaders[i]->GetProgram();
		lightedShaders[i]->Use();
		SetPointLightMain(program, position, pointParams, amb, diff, spec);
	}

	bulbColor = diff;
	bulbModelMat = translate(bulbModelMat, position);
	bulbModelMat = scale(bulbModelMat, vec3(0.5f, 0.5f, 0.5f));
}

void MyLight::SetSpotLightMain(GLuint prog, vec3 position, vec3 direction, vec3 pointParams, vec2 spotParams, vec3 amb, vec3 diff, vec3 spec)
{
	std::string number = std::to_string(id);

	glUniform1i(glGetUniformLocation(prog, ("lights[" + number + "].active").c_str()), 1);
	glUniform1i(glGetUniformLocation(prog, ("lights[" + number + "].type").c_str()), 2);

	glUniform1f(glGetUniformLocation(prog, ("lights[" + number + "].constant").c_str()), pointParams.x);
	glUniform1f(glGetUniformLocation(prog, ("lights[" + number + "].linear").c_str()), pointParams.y);
	glUniform1f(glGetUniformLocation(prog, ("lights[" + number + "].quadratic").c_str()), pointParams.z);

	glUniform1f(glGetUniformLocation(prog, ("lights[" + number + "].cutoff").c_str()), spotParams.x);
	glUniform1f(glGetUniformLocation(prog, ("lights[" + number + "].outercutoff").c_str()), spotParams.y);

	glUniform3fv(glGetUniformLocation(prog, ("lights[" + number + "].position").c_str()), 1, &position[0]);
	glUniform3fv(glGetUniformLocation(prog, ("lights[" + number + "].direction").c_str()), 1, &direction[0]);

	glUniform3fv(glGetUniformLocation(prog, ("lights[" + number + "].ambient").c_str()), 1, &amb[0]);
	glUniform3fv(glGetUniformLocation(prog, ("lights[" + number + "].diffuse").c_str()), 1, &diff[0]);
	glUniform3fv(glGetUniformLocation(prog, ("lights[" + number + "].specular").c_str()), 1, &spec[0]);
}

void MyLight::SetSpotLight(vec3 position, vec3 direction, vec3 pointParams, vec2 spotParams, vec3 amb, vec3 diff, vec3 spec)
{
	//point params -> (k_c, k_l, k_q)
	//spot params -> (inner cutoff, outer cutoff)

	//register with default opaque renderer (the one without texturing)
	GLuint program;

	for (unsigned int i = 0; i < lightedShaders.size(); i++)
	{
		program = lightedShaders[i]->GetProgram();
		lightedShaders[i]->Use();
		SetSpotLightMain(program, position, direction, pointParams, spotParams, amb, diff, spec);
	}

	bulbColor = diff;
	bulbModelMat = translate(bulbModelMat, position);
	bulbModelMat = scale(bulbModelMat, vec3(0.5f, 0.5f, 0.5f));
}

void MyLight::Display()
{
	//display bulb if it is enabled
	if (isBulb)
	{
		bulbShader->Use();

		GLuint program = bulbShader->GetProgram();

		glUniform3fv(glGetUniformLocation(program, "bulbColor"), 1, &bulbColor[0]);

		glm::mat4 view = Scene::GetCamera()->GetViewMatrix();
		glm::mat4 projection = Scene::GetCamera()->GetProjectionMatrix();

		//set shader uniforms
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &bulbModelMat[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, &projection[0][0]);

		glBindVertexArray(bulbVAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
}

void MyLight::Display(Shader* shader)
{
	shader->Use();

	glUniformMatrix4fv(glGetUniformLocation(shader->GetProgram(), "model"), 1, GL_FALSE, &bulbModelMat[0][0]);

	glBindVertexArray(bulbVAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}


void MyLight::Update(const GLfloat& deltaTime)
{
}