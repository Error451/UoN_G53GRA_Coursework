#pragma once

// GLEW
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>

class Engine
{
public:
	Engine();
	Engine(int argc, char** argv, const char* title, const int& windowWidth, const int& windowHeight);
	
	virtual ~Engine(){}

	virtual void Run();

	static Engine* GetCurrentInstance();

	static const char* GetWindowTitle();

	static GLFWwindow* GetWindow();

	static int GetWindowWidth();

	static int GetWindowHeight();

	static int CheckGLError();

	static GLfloat _runtime;
	//the total time spend after the render loop start
	static GLfloat _renderTime;
	//delta time between two rendering call
	static GLfloat _renderDT;

protected:
	static Engine *_current;

	static const char* _windowTitle;
	static GLFWwindow* _window;

	static GLfloat _errorTimer;
	static int _windowWidth;
	static int _windowHeight;

	static void InitFunc();
	static void RenderingFunc();
	static void IdleFunc();
	static void CreateWindow();
	static void SetupViewport();
	static void SetCallbacks();		
	static void KeyCallback(GLFWwindow* _window, int key, int scancode, int action, int mode);

	virtual void Initialise() = 0;
	virtual void Draw() = 0;
	virtual void Update(const GLfloat&) = 0;
	virtual void HandleKey(int, int, int) = 0;
		
	static std::string GetErrorString(GLenum error);
};

