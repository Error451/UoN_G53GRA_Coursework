#include "InputHandler.h"

InputHandler* InputHandler::_current = nullptr;
GLFWcursor* InputHandler::_cursor = nullptr;
GLboolean InputHandler::_keyStates[1024];
GLboolean InputHandler::_mouseButtonStates[8];

GLchar InputHandler::_modState  = 0x0000;

GLdouble InputHandler::_mouseX  = 0.0;
GLdouble InputHandler::_mouseY  = 0.0;
GLdouble InputHandler::_scrollX = 0.0;
GLdouble InputHandler::_scrollY = 0.0;

GLboolean InputHandler::_cursorEnterState = 0;

InputHandler::InputHandler()
{
	InputHandler::_current = this;
	
	for (int i = 0; i < 1024; ++i)
		InputHandler::_keyStates[i] = GLFW_RELEASE;
	
	for (int i = 0; i < 8; ++i)
		InputHandler::_mouseButtonStates[i] = GLFW_RELEASE;

}

InputHandler::~InputHandler()
{

}

// public:

void InputHandler::SetCallbacks(GLFWwindow* window)
{
	glfwSetKeyCallback(window, InputHandler::KeyCallback);
	glfwSetMouseButtonCallback(window, InputHandler::MouseCallback);
	glfwSetCursorEnterCallback(window, InputHandler::CursorEnterCallback);
	glfwSetCursorPosCallback(window, InputHandler::CursorCallback);
	glfwSetScrollCallback(window, InputHandler::ScrollCallback);

	glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
}

InputHandler* InputHandler::GetInputHandler()
{
	return _current;
}

GLboolean InputHandler::GetKeyState(GLint key)
{
	InputHandler::_keyStates[key] = glfwGetKey(Engine::GetWindow(), key);
	return InputHandler::_keyStates[key];
}

GLboolean InputHandler::GetMouseState(GLint button)
{
	InputHandler::_mouseButtonStates[button] = glfwGetMouseButton(Engine::GetWindow(), button);
	return InputHandler::_mouseButtonStates[button];
}

GLchar InputHandler::GetModState()
{
	return InputHandler::_modState;
}

GLboolean InputHandler::GetCursorState()
{
	return InputHandler::_cursorEnterState;
}

GLdouble InputHandler::GetMouseX()
{
	return InputHandler::_mouseX;
}

GLdouble InputHandler::GetMouseY()
{
	return InputHandler::_mouseY;
}

GLdouble InputHandler::GetScrollX()
{
	return InputHandler::_scrollX;
}

GLdouble InputHandler::GetScrollY()
{
	return InputHandler::_scrollY;
}

// private:
void InputHandler::KeyCallback(GLFWwindow* window, GLint key, GLint scancode, GLint action, GLint mods)
{
	if (key >= 0 && key < 1024)
		InputHandler::_keyStates[key] = action;

	InputHandler::_modState = static_cast<GLchar>(mods);

	if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

void InputHandler::MouseCallback(GLFWwindow* window, GLint button, GLint action, GLint mods)
{
	InputHandler::_mouseButtonStates[button] = action;
	InputHandler::_modState = static_cast<GLchar>(mods);
}

void InputHandler::ScrollCallback(GLFWwindow* window, GLdouble xOffset, GLdouble yOffset)
{
	InputHandler::_scrollX += xOffset;
	InputHandler::_scrollY += yOffset;
}

void InputHandler::CursorCallback(GLFWwindow* window, GLdouble xpos, GLdouble ypos)
{
	InputHandler::_mouseX = xpos;
	InputHandler::_mouseY = ypos;
}

void InputHandler::CursorEnterCallback(GLFWwindow* window, GLint state)
{
	InputHandler::_cursorEnterState = static_cast<GLboolean>(state);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	if (state)
	{
		InputHandler::_cursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
		glfwSetCursor(window, _cursor);
	}
	else
	{
		glfwDestroyCursor(_cursor);
		glfwSetCursor(window, nullptr);
	}
}