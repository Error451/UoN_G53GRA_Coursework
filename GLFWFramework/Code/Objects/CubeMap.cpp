#include "CubeMap.h"
#include "Scene.h"
#include "SimpleObjects.h"

#include <stb_image.h>

CubeMap::CubeMap(Shader* renderer)
	:cmRenderer(renderer)
{
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glGenBuffers(1, &cubeEBO);

	glBindVertexArray(cubeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(simpleCubeVerts), simpleCubeVerts, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(simpleCubeIndices), simpleCubeIndices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

CubeMap::~CubeMap()
{
}

void CubeMap::Display()
{
	//reverse triangle winding
	glFrontFace(GL_CW);
	//disable depth writing
	glDepthMask(GL_FALSE);
	cmRenderer->Use();
	GLuint program = cmRenderer->GetProgram();

	//remove any translation, so the skybox cube is always at the origin in the view space
	mat4 view = mat4(mat3(Scene::GetCamera()->GetViewMatrix()));
	//mat4 view = Scene::GetCamera()->GetViewMatrix();
	mat4 projection = Scene::GetCamera()->GetProjectionMatrix();

	//set shader uniforms
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, &projection[0][0]);

	glBindVertexArray(cubeVAO);
	glUniform1i(glGetUniformLocation(program, "cubemap"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapId);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

	//unbind everything
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glBindVertexArray(0);

	glDepthMask(GL_TRUE);
	//restore default winding
	glFrontFace(GL_CCW);
}

void CubeMap::loadCubeMap(std::vector<std::string> faces)
{
	GLuint texId;
	
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texId);

	unsigned char* image;
	GLsizei width;
	GLsizei height;

	//load cubemap textures
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		stbi_set_flip_vertically_on_load(0);
		image = stbi_load(faces[i].c_str(), &width, &height, 0, 0);
		//image = SOIL_load_image("./Textures/starfield_bk.tga", &width, &height, 0, SOIL_LOAD_AUTO);
		if (image)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, 
				width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
			//std::cout << "aaaaaaaa" << std::endl;
		}
		else
		{
			std::cout << "Cubemap texture fail to load by name: " << faces[i] << " : " << stbi_failure_reason() << std::endl;
		}
		stbi_image_free(image);
	}
	//SOIL_free_image_data(image);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	
	cubemapId = texId;
}

