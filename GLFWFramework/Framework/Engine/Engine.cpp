#include "Engine.h"
#include "InputHandler.h"
#include <iostream>

#define _CHECK_ERROR_RATE 5.0f

Engine *Engine::_current = 0;
GLFWwindow *Engine::_window = nullptr;

const char* Engine::_windowTitle = "";
int Engine::_windowWidth = 0;
int Engine::_windowHeight = 0;

//timer
GLfloat Engine::_runtime = 0.0;
GLfloat Engine::_renderTime = 0.0;	//run time logger for rendering function
GLfloat Engine::_renderDT = 0.0;	//delta time between two rendering call

GLfloat Engine::_errorTimer = 0;

Engine::Engine()
{
	glfwInit();
}

Engine::Engine(int argc, char** argv, const char *title, const int &windowWidth, const int &windowHeight)
	: Engine()
{
	Engine::_windowHeight = windowHeight;
	Engine::_windowWidth = windowWidth;
	Engine::_windowTitle = title;
}

void Engine::Run()
{
	CreateWindow();
	_current = this;

	SetupViewport();

	SetCallbacks();

	InitFunc();
	
	std::cout << "Setup complete. Window created [" << _windowWidth << "x" << _windowHeight << "]" << std::endl;
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

	InputHandler::InputHandler();
	InputHandler::SetCallbacks(Engine::GetWindow());

	std::cout << "Error output rate: " << _CHECK_ERROR_RATE << " seconds" << std::endl;
	CheckGLError();

	while (!glfwWindowShouldClose(_window))
	{
		// Check and call events
		glfwPollEvents();

		// Update runtime
		IdleFunc();

		// Rendering code
		RenderingFunc();
	}

	// Check and print errors
	CheckGLError();

	glfwTerminate();
}

void Engine::InitFunc()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glEnable(GL_LINE_SMOOTH);

	_current->Initialise();
	_runtime = 0.0f;
}

void Engine::IdleFunc()
{
	GLfloat currentTime = static_cast<GLfloat>(glfwGetTime());

	float deltaTime;
	deltaTime = (currentTime - _runtime);

	_runtime = currentTime;

	_current->Update(deltaTime);

	Engine::_errorTimer += deltaTime;
	if (_errorTimer > _CHECK_ERROR_RATE)
	{
		CheckGLError();
		Engine::_errorTimer = 0.0f;
	}
}

void Engine::RenderingFunc()
{
	GLfloat currentTime = static_cast<GLfloat>(glfwGetTime());
	_renderDT = (currentTime - _renderTime);

	_renderTime = currentTime;

	_current->Draw();

	// Swap the buffers
	glfwSwapBuffers(_window);
}

void Engine::CreateWindow()
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	//enabling MSAA 4x
	glfwWindowHint(GLFW_SAMPLES, 4);
	glEnable(GL_MULTISAMPLE);

	_window = glfwCreateWindow(_windowWidth, _windowHeight, _windowTitle, nullptr, nullptr);
	if (_window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(_window);
	
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		exit(-1);
	}
}

void Engine::SetCallbacks()
{
	// Add window callbacks here
	glfwSetKeyCallback(_window, KeyCallback);
}

void Engine::SetupViewport()
{
	int width, height;
	glfwGetFramebufferSize(_window, &width, &height);
	glViewport(0, 0, width, height);

	Engine::_windowHeight = height;
	Engine::_windowWidth = width;
}

void Engine::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// When a user presses the escape key, we set the WindowShouldClose property to true,
	// closing the application
	if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
		return;
	}
	_current->HandleKey(key, action, mode);
}

Engine* Engine::GetCurrentInstance()
{
	return Engine::_current;
}

const char* Engine::GetWindowTitle()
{
	return Engine::_windowTitle;
}

GLFWwindow* Engine::GetWindow()
{
	return Engine::_window;
}

int Engine::GetWindowWidth()
{
	return Engine::_windowWidth;
}

int Engine::GetWindowHeight()
{
	return Engine::_windowHeight;
}

int Engine::CheckGLError()
{
	int e = 0;
	GLenum error = glGetError();
	while (GL_NO_ERROR != error)
	{
		++e;
		std::cout << "GL Error " << e << ": " << GetErrorString(error) << std::endl;
		error = glGetError();
	}
	return e;
}

std::string Engine::GetErrorString(GLenum error)
{
	std::string errorString;

	switch (error)
	{
	case GL_NO_ERROR:
		errorString = "GL_NO_ERROR";
		break;
	case GL_INVALID_ENUM:
		errorString = "GL_INVALID_ENUM";
		break;
	case GL_INVALID_VALUE:
		errorString = "GL_INVALID_VALUE";
		break;
	case GL_INVALID_OPERATION:
		errorString = "GL_INVALID_OPERATION";
		break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		errorString = "GL_INVALID_FRAMEBUFFER_OPERATION";
		break;
	case GL_OUT_OF_MEMORY:
		errorString = "GL_OUT_OF_MEMORY";
		break;
	case GL_STACK_UNDERFLOW:
		errorString = "GL_STACK_UNDERFLOW";
		break;
	case GL_STACK_OVERFLOW:
		errorString = "GL_STACK_OVERFLOW";
		break;
	}

	return errorString;
}