#pragma once

#include <string>
#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "DisplayableObject.h"
#include "Mesh.h"

class Model
	:public DisplayableObject
{
public:
	Model(Shader* prog, char *path);
	~Model() = default;

	void Display();
	void Update(const GLfloat& deltaTime);

	void Translate(glm::vec3 trans);
	void Scale(glm::vec3 scale);
	void Rotate(float angle, glm::vec3 axis);

private:
	Shader* shader;

	vector<Mesh> meshes;
	string directory;

	void loadModel(string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	vector<MeshTexture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, int targetType);
	float loadMaterialShininess(aiMaterial* mat);

	mat4 model;

	//animation helper variables
	float lastSin = 0.0f;
	bool _velocitybreak;
	bool _xpress1 = false;
	bool _xrelease1 = true;
	bool _xpress2 = false;
	bool _xrelease2 = true;
	bool _bFreeze = false;
	int movecount1 = 0;
};

