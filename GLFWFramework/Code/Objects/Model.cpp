#include "Model.h"
#include "InputHandler.h"
#include "Header.h"

Model::Model(Shader* prog, char *path)
	:_velocitybreak(false)
{
	loadModel(path);
	shader = prog;
	model = mat4(1.0f);
}

void Model::Display()
{
	shader->Use();
	GLuint program = shader->GetProgram();

	glm::mat4 view = Scene::GetCamera()->GetViewMatrix();
	glm::mat4 projection = Scene::GetCamera()->GetProjectionMatrix();

	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, &model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, &projection[0][0]);

	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Draw(shader);
	}
}

void Model::Update(const GLfloat& deltaTime)
{
	if (InputHandler::GetKeyState(GLFW_KEY_P))
	{
		if (_xrelease2)
		{
			_xpress2 = true;
		}
		_xrelease2 = false;
	}
	else
	{
		if (_xpress2)
		{
			_xrelease2 = true;

			_bFreeze = !_bFreeze;
			//std::cout << bFreeze << std::endl;
		}
		_xpress2 = false;
	}

	if (_bFreeze)
	{
		//skip current step
		return;
	}

	float displacement = sin(1.5 * Engine::_renderTime);

	this->Translate(vec3(0.0f, displacement * 1.5, 0.0f));
	this->Rotate((displacement - lastSin) * 2, vec3(0.0f, 0.0f, 1.0f));

	lastSin = displacement;

	if (InputHandler::GetKeyState(GLFW_KEY_UP))
	{
		if (_xrelease1)
		{
			_xpress1 = true;
		}
		_xrelease1 = false;

		_velocitybreak = false;
		
		
		if (movecount1 < 150)
		{
			this->Translate(vec3(0.0f, 0.0f, 0.05f * (150 - movecount1)));
			movecount1++;
		}
	}
	else
	{
		if (_xpress1)
		{
			_xrelease1 = true;

			_velocitybreak = true;
		}
		_xpress1 = false;
	}

	if (_velocitybreak)
	{
		if (movecount1 > 0)
		{
			this->Translate(vec3(0.0f, 0.0f, -0.05f * movecount1));
			movecount1--;
		}
	}
}

void Model::loadModel(string path)
{
	Assimp::Importer importer;
	//load model, reversing texture uv and try to convert all faces to triangle if it is not
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

	//if any error occurs, print error message
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "ERROR::ASSIMP::" << importer.GetErrorString() << endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	//process all the node's mesh (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		//retrive mesh based on indices
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	//then do the same for each node in the subtree
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		//recursion
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<MeshTexture> textures;

	glm::vec3 vector3;
	glm::vec2 vector2;
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		//process vertex positions, normals and texture coordinates
		//position
		vector3.x = mesh->mVertices[i].x;
		vector3.y = mesh->mVertices[i].y;
		vector3.z = mesh->mVertices[i].z;
		vertex.position = vector3;
		//normal
		vector3.x = mesh->mNormals[i].x;
		vector3.y = mesh->mNormals[i].y;
		vector3.z = mesh->mNormals[i].z;
		vertex.normal = vector3;
		//check if texture coords is included in model and try to load
		if (mesh->mTextureCoords[0])
		{
			//only extract first texture coordinates set
			vector2.x = mesh->mTextureCoords[0][i].x;
			vector2.y = mesh->mTextureCoords[0][i].y;
			vertex.texcoords = vector2;
		}
		else
		{
			vertex.texcoords = vec2(0.0f, 0.0f);
		}

		vertices.push_back(vertex);
	}
	//process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	float matShininess;
	//process material
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		//diffuse maps
		vector<MeshTexture> diffuseMaps = loadMaterialTextures(material,
			aiTextureType_DIFFUSE, TEXTURE_DIFFUSE);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		//specular map
		vector<MeshTexture> specularMaps = loadMaterialTextures(material,
			aiTextureType_SPECULAR, TEXTURE_SPECULAR);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		vector<MeshTexture> glowMaps = loadMaterialTextures(material,
			aiTextureType_EMISSIVE, TEXTURE_EMISSIVE);
		textures.insert(textures.end(), glowMaps.begin(), glowMaps.end());

		matShininess = loadMaterialShininess(material);
	}

	return Mesh(vertices, indices, textures, matShininess);
}

vector<MeshTexture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, int targetType)
{
	vector<MeshTexture> textures;
	aiString str;

	Texture* texHandle = new Texture();

	//retrieve texture from file, duplicate texture will be handled by loader (Texture class)
	//only working for file format that store textures separately (like .obj)
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		mat->GetTexture(type, i, &str);

		//store current directory for loading textures
		string path = directory + "/" + (string)str.C_Str();

		MeshTexture texture;
		GLuint texId = texHandle->GetTexture(path);
		texture.id = texId;
		texture.type = targetType;

		textures.push_back(texture);
	}

	return textures;
}

float Model::loadMaterialShininess(aiMaterial* mat)
{
	//return shininess
	float holder;
	mat->Get(AI_MATKEY_SHININESS, holder);

	return holder;
}

//same member function as cube, maybe need some hierarchy but I'm lazy (DisplayableObject shouldn't be abstract any more)
void Model::Translate(glm::vec3 trans)
{
	model = glm::translate(model, trans);
}

void Model::Scale(glm::vec3 scale)
{
	model = glm::scale(model, scale);
}

//angle in degree
void Model::Rotate(float angle, glm::vec3 axis)
{
	//_model = glm::rotate(_model, angle, axis);
	model = myRotate(model, angle, axis);
}