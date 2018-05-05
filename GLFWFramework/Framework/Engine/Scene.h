#pragma once

#include "Engine.h"

#include <vector>

#include "DisplayableObject.h"
#include "Input.h"
#include "Texture.h"
#include "Camera.h"


class DisplayableObject;
class Camera;

class Scene :
	public Engine
{
public:
	
	Scene(int argc, char** argv, const char* title, const int& windowWidth, const int& windowHeight);
	
	virtual ~Scene();

	static GLuint GetTexture(std::string filename);
	static Camera* GetCamera();

	//draw a full screen quad, require corresponding shaders
	static void DrawFullScreenQuad();
	//get default texture - grey
	static GLuint GetDefaultTexture() { return texDefault; };

protected:
	virtual void Initialise();

	void Draw();

	void Update(const GLfloat& deltaTime);

	void AddObjectToScene(DisplayableObject* obj);

	void HandleKey(int key, int action, int mods);

protected:

	std::vector<DisplayableObject*> _objects;

	static Camera _camera;

//utility functions
protected:
	static GLuint fullScreenQuadVAO;
	//set up full screen quad
	static void InitFullScreenQuad();

	/* The function will return loaded texture id
	*  just in case it is useful
	*/
	GLuint LoadTexture(std::string path, std::string tag);

	//pure virtual texture loading function
	virtual void LoadTextures() = 0;

	Texture texHandler;
	//default texture
	static GLuint texDefault;
	
	//texture list
	std::map<std::string, GLuint> myTextures;

	//insert texture with tag
	void AddTextureToTheScene(std::string tag, GLuint t)
	{ myTextures.insert(std::pair<std::string, GLuint>(tag, t)); };
};

