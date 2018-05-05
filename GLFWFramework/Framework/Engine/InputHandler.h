#pragma once

#include "Engine.h"

class InputHandler
{
public:
	InputHandler();
	~InputHandler();

	static void SetCallbacks(GLFWwindow* window);

	static InputHandler* GetInputHandler();

	static GLboolean GetKeyState(GLint key);
	static GLboolean GetMouseState(GLint button);
	static GLchar GetModState();
	static GLboolean GetCursorState();

	static GLdouble GetMouseX();
	static GLdouble GetMouseY();
	static GLdouble GetScrollX();
	static GLdouble GetScrollY();

private:
	static InputHandler* _current;

	static GLboolean _keyStates[1024];
	static GLboolean _mouseButtonStates[8];

	static GLchar _modState;
	static GLboolean _cursorEnterState;

	static GLdouble _mouseX;
	static GLdouble _mouseY;

	static GLdouble _scrollX;
	static GLdouble _scrollY;

	static GLFWcursor* _cursor;

	static void KeyCallback(GLFWwindow* window, GLint key, GLint scancode, GLint action, GLint mods);
	static void MouseCallback(GLFWwindow* window, GLint button, GLint action, GLint mods);
	static void ScrollCallback(GLFWwindow* window, GLdouble xOffset, GLdouble yOffset);
	static void CursorCallback(GLFWwindow* window, GLdouble xpos, GLdouble ypos);
	static void CursorEnterCallback(GLFWwindow* window, GLint state);
};

