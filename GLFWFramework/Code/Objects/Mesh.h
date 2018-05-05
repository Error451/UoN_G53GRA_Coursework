#pragma once

#include <string>
#include <vector>

#include "DisplayableObject.h"

#define TEXTURE_DIFFUSE 0
#define TEXTURE_SPECULAR 1
#define TEXTURE_EMISSIVE 2

using namespace glm;
using namespace std;

struct Vertex
{
	vec3 position;
	vec3 normal;
	vec2 texcoords;
};

//type 0 -> diffuse 1-> specular
struct MeshTexture
{
	GLuint id;
	int type;
};

class Mesh
{
public:

	//Mesh data
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<MeshTexture> textures;

	//functions
	Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<MeshTexture> textures, float shininess);
	~Mesh();

	void Draw(Shader* shader);

private:
	//render data
	GLuint VAO, VBO, EBO;

	float shininess;

	//functions
	void setupMesh();

	string nameDiff = "texture_diffuse";
	string nameSpec = "texture_specular";
	string nameEmis = "texture_emissive";
};

