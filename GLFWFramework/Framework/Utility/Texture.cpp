#include "Texture.h"


#include <stb_image.h>
#include <iostream>

using namespace std;

Texture::Texture()
{
}


Texture::~Texture()
{
	GLuint tex;
	for (map<string, GLuint>::iterator it = _textures.begin(); it != _textures.end(); ++it)
	{
		tex = it->second;
		glDeleteTextures(1, &tex);
	}
}

GLuint Texture::GetTexture(string filename)
{
	if (_textures.find(filename) != _textures.end())
		return _textures[filename];

	int width, height, nchannel;

	// tell stb_image.h to flip loaded texture's on the y-axis.
	stbi_set_flip_vertically_on_load(1);

	unsigned char* image = stbi_load(filename.c_str(), &width, &height, &nchannel, 0);

	if (image == NULL)
	{
		std::cout << "Texture.cpp: Error, unable to read image " << filename << ": " << stbi_failure_reason() << std::endl;
		return NULL;
	}
	
	GLenum format;
	if (nchannel == 1)
		format = GL_RED;
	else if (nchannel == 3)
		format = GL_RGB;
	else if (nchannel == 4)
		format = GL_RGBA;

	GLuint texture = 0;
	glGenTextures(1, &texture);

	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(image);
	glBindTexture(GL_TEXTURE_2D, 0);

	_textures[filename] = texture;

	return texture;
}