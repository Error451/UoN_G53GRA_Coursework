#pragma once

#include <GL/glew.h>

#include <map>
#include <string>

class Texture
{
public:
	Texture();
	~Texture();

	GLuint GetTexture(std::string filename);

private:
	std::map<std::string, GLuint> _textures;
};