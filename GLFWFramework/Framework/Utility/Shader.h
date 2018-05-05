#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>

class Shader
{
public:
	Shader();
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	Shader(const GLchar* vertexPath, const GLchar* geometryPath, const GLchar* fragmentPath);
	~Shader(){}

	void Use();

	inline GLuint GetProgram() { return _program; }

	void AddVertexShader(const GLchar* vertexPath);
	void AddFragmentShader(const GLchar* fragmentPath);
	void DetachShader(const GLenum& shaderType);

private:
	GLuint _program;

	void CompileShader(GLuint *shader, const char* sourcePath, const char* tag);
	std::string ReadShaderFile(const char* filepath);
	
	void CheckShaderCompilation(const GLuint *shader, const char* tag);
	void CheckProgramLink();

};

