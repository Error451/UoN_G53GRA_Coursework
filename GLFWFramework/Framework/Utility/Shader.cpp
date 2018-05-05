#include "Shader.h"
#include "Scene.h"

Shader::Shader()
{
	_program = glCreateProgram();
}

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath) : Shader()
{
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	CompileShader(&vertexShader, vertexPath, vertexPath);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	CompileShader(&fragmentShader, fragmentPath, fragmentPath);

	glAttachShader(_program, vertexShader);
	glAttachShader(_program, fragmentShader);
	glLinkProgram(_program);
	CheckProgramLink();

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

Shader::Shader(const GLchar* vertexPath, const GLchar* geometryPath, const GLchar* fragmentPath)
{
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	CompileShader(&vertexShader, vertexPath, vertexPath);

	GLuint geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
	CompileShader(&geometryShader, geometryPath, geometryPath);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	CompileShader(&fragmentShader, fragmentPath, fragmentPath);

	glAttachShader(_program, vertexShader);
	glAttachShader(_program, geometryShader);
	glAttachShader(_program, fragmentShader);
	glLinkProgram(_program);
	CheckProgramLink();

	glDeleteShader(vertexShader);
	glDeleteShader(geometryShader);
	glDeleteShader(fragmentShader);
}

void Shader::DetachShader(const GLenum& shaderType)
{
	switch (shaderType)
	{
	case GL_VERTEX_SHADER:
	case GL_FRAGMENT_SHADER:
	case GL_GEOMETRY_SHADER:
		break;
	default:
		std::cout << "Warning: Invalid Shader Type. Cannot detach." << std::endl;
		return;
	}

	const GLsizei MAXCOUNT = 3;
	GLsizei num_shaders;
	GLuint shaders[MAXCOUNT];
	for (int i = 0; i < MAXCOUNT; ++i)
		shaders[i] = NULL;

	glGetAttachedShaders(_program, MAXCOUNT, &num_shaders, &shaders[0]);

	for (GLuint s : shaders)
	{
		if (s != NULL)
		{
			GLint param;
			glGetShaderiv(s, GL_SHADER_TYPE, &param);
			if (param == shaderType)
			{
				glDetachShader(_program, s);
			}
		}
	}
}

void Shader::AddVertexShader(const GLchar* vertexPath)
{
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	CompileShader(&vertexShader, vertexPath, vertexPath);

	DetachShader(GL_VERTEX_SHADER);
	glAttachShader(_program, vertexShader);
	glLinkProgram(_program);
	CheckProgramLink();

	glDeleteShader(vertexShader);
}

void Shader::AddFragmentShader(const GLchar* fragmentPath)
{
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	CompileShader(&fragmentShader, fragmentPath, fragmentPath);

	DetachShader(GL_FRAGMENT_SHADER);

	glAttachShader(_program, fragmentShader);

	glLinkProgram(_program);
	CheckProgramLink();

	glDeleteShader(fragmentShader);
}

void Shader::Use()
{
	glUseProgram(_program);
}

void Shader::CompileShader(GLuint *shader, const char* sourcePath, const char* tag)
{
	std::string sourcecode = ReadShaderFile(sourcePath);
	const GLchar* shaderCode = sourcecode.c_str();

	glShaderSource(*shader, 1, &shaderCode, NULL);
	glCompileShader(*shader);

	CheckShaderCompilation(shader, tag);
}

std::string Shader::ReadShaderFile(const char* filepath)
{
	std::ifstream shaderFile;
	std::stringstream shaderStream;

	shaderFile.exceptions(std::ifstream::badbit);
	try
	{
		shaderFile.open(filepath);
		std::stringstream ss;
		shaderStream << shaderFile.rdbuf();
		shaderFile.close();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "Failed to read shader file " << filepath << std::endl;
	}

	return shaderStream.str();
}

void Shader::CheckShaderCompilation(const GLuint *shader, const char* tag)
{
	GLint success;
	GLchar infoLog[512];

	glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(*shader, 512, NULL, infoLog);
		std::cout << "ERROR : " << tag << " compilation failed\n" << infoLog << std::endl;
	}
	else
	{
		std::cout << ">>" << tag << "<< " << " compiled successfully." << std::endl;
	}

}

void Shader::CheckProgramLink()
{
	GLint success;
	GLchar infoLog[512];

	glGetProgramiv(_program, GL_LINK_STATUS, &success);

	if (!success)
	{
		glGetProgramInfoLog(_program, 512, NULL, infoLog);
		std::cout << "ERROR : Shader program link failed\n" << infoLog << std::endl;
	}
	else
	{
		std::cout << "Shader program linked successfully." << std::endl;
	}
}