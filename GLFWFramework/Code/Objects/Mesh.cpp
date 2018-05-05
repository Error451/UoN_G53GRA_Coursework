#include "Mesh.h"

#include <iostream>
#include <stddef.h>

Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<MeshTexture> textures, float shininess)
	:shininess(shininess)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	setupMesh();
}

//load mesh data to buffers
void Mesh::setupMesh()
{
	//regular set up procedures, using offset instead of individual buffers for each attribute ->
	// -> (less efficient ? but simpler)
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	//posiitons
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	//normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	//texture coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoords));

	glBindVertexArray(0);
}

void Mesh::Draw(Shader* shader)
{
	//++++++++++++++++++++++++++++++++++++++++++++++++++
	//shader and MVP matrices will be set at model class
	//++++++++++++++++++++++++++++++++++++++++++++++++++

	GLuint program = shader->GetProgram();

	GLuint diffuseNr = 1;
	GLuint specularNr = 1;
	GLuint emissiveNr = 1;

	string number, name;
	int type;

	//setup texture before rendering
	for (GLuint i = 0; i < textures.size(); i++)
	{
		//find sampler name by texture type
		type = textures[i].type;
		switch (type)
		{
		case TEXTURE_DIFFUSE:
			name = nameDiff;
			number = std::to_string(diffuseNr++);
			break;
		case TEXTURE_SPECULAR:
			name = nameSpec;
			number = std::to_string(specularNr++);
			break;
		case TEXTURE_EMISSIVE:
			name = nameEmis;
			number = std::to_string(emissiveNr++);
			break;
		default:
			break;
		}
		
		glUniform1i(glGetUniformLocation(program, ("material." + name + number).c_str()), i); //assign texture unit to sampler
		glActiveTexture(GL_TEXTURE0 + i);	//active texture
		glBindTexture(GL_TEXTURE_2D, textures[i].id);	//assign texture id to texture unit
	}
	
	glUniform1f(glGetUniformLocation(program, "material.shininess"), shininess);

	//draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	//unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
}

Mesh::~Mesh()
{
}
