#include "Camera.h"

#include "Engine.h"
#include "InputHandler.h"
#include <iostream>

#define _MOVEMENT_SPEED 16.0f;
#define _MOUSE_SENSITIVITY 10.0f;


Camera::Camera() : _flagFirstCallback(true), _aspect(4.0f / 3.0f)
{
	_prevX = static_cast<GLfloat>(Engine::GetWindowWidth()) / 2.0f;
	_prevY = static_cast<GLfloat>(Engine::GetWindowHeight()) / 2.0f;

	_prevScrollX = static_cast<GLfloat>(InputHandler::GetScrollX());
	_prevScrollY = static_cast<GLfloat>(InputHandler::GetScrollY());
	Reset();
}

void Camera::Reset()
{
	//eye: -13, 7.7 9.0
	//view: 0.52 -0.41 -0.746

	_eyePosition = glm::vec3(0.0f, 0.0f, 5.0f);
	//_eyePosition = glm::vec3(-13.0f, 7.7f, 9.0f);
	//_viewDirection = glm::vec3(0.52f, -0.41f, -0.746f);
	_viewDirection = glm::vec3(0.0f, 0.0f, -1.0f);

	_right = glm::vec3(1.0f, 0.0f, 0.0f);
	_up = glm::vec3(0.0f, 1.0f, 0.0f);
	_forward = glm::vec3(0.0f, 0.0f, -1.0f);

	_rise = glm::vec3(0.0f, 1.0f, 0.0f);

	_yaw   = -90.0f;
	_pitch = 0.0f;

	_fovy = glm::radians(45.0f);

	SetupCamera();
}

glm::mat4 Camera::GetViewMatrix()
{
	return _viewMatrix;
}

glm::mat4 Camera::GetProjectionMatrix()
{
	return _projectionMatrix;
}

void Camera::SetupCamera()
{
	_viewMatrix = glm::lookAt(_eyePosition,
							  _eyePosition + _viewDirection,
							  _up);

	_projectionMatrix = glm::perspective(_fovy, _aspect, 0.1f, 1000.0f);
}	

void Camera::Update(const GLfloat &deltaTime)
{
	if (InputHandler::GetKeyState(GLFW_KEY_SPACE))
		Reset();

	HandleMouseMove(deltaTime);

	HandleKeyMovement(deltaTime);
	
	_aspect = static_cast<GLfloat>(Engine::GetWindowWidth()) / static_cast<GLfloat>(Engine::GetWindowHeight());

	SetupCamera();
}

void Camera::HandleKeyMovement(const GLfloat& deltaTime)
{
	GLfloat speed = deltaTime * _MOVEMENT_SPEED;

	if (InputHandler::GetKeyState(GLFW_KEY_A))
		_eyePosition -= _right*speed;

	if (InputHandler::GetKeyState(GLFW_KEY_D))
		_eyePosition += _right*speed;

	if (InputHandler::GetKeyState(GLFW_KEY_W))
		_eyePosition += _forward*speed;

	if (InputHandler::GetKeyState(GLFW_KEY_S))
		_eyePosition -= _forward*speed;

	if (InputHandler::GetKeyState(GLFW_KEY_LEFT_ALT))
		_eyePosition += _rise * speed;

	if (InputHandler::GetKeyState(GLFW_KEY_LEFT_SHIFT))
		_eyePosition -= _rise * speed;

	if (InputHandler::GetKeyState(GLFW_KEY_TAB))
	{
		std::cout << "eye position: " << "(" << _eyePosition.x << ", " << _eyePosition.y << ", " << _eyePosition.z << ")" << std::endl;
		std::cout << "view direction: " << "(" << _viewDirection.x << ", " << _viewDirection.y << ", " << _viewDirection.z << ")" << std::endl;
		std::cout << "up vector: " << "(" << _up.x << ", " << _up.y << ", " << _up.z << ")" << std::endl;
	}
		
}

void Camera::HandleMouseScroll(const GLfloat& deltaTime)
{
	// Doesn't work
	GLfloat scrollY = static_cast<GLfloat>(InputHandler::GetScrollY());
	GLfloat offset = scrollY - _prevScrollY;
	
	if (abs(offset) > 0.0f)
	{
		if (_fovy >= 1.0f && _fovy <= 45.0f)
			_fovy -= scrollY;

		if (_fovy < 1.0f)
			_fovy = 1.0f;
		else if (_fovy > 45.0f)
			_fovy = 45.0f;

	}
	_prevScrollY = scrollY;
}

void Camera::HandleMouseMove(const GLfloat& deltaTime)
{
	GLdouble mx = InputHandler::GetMouseX();
	GLdouble my = InputHandler::GetMouseY();

	if (_flagFirstCallback)
	{
		Reset();
		_prevX = mx;
		_prevY = my;
		_flagFirstCallback = false;
	}

	GLfloat xoffset = static_cast<GLfloat>(mx - _prevX);
	GLfloat yoffset = static_cast<GLfloat>(my - _prevY);

	xoffset *= deltaTime * _MOUSE_SENSITIVITY;
	yoffset *= deltaTime * _MOUSE_SENSITIVITY;

	if (InputHandler::GetMouseState(GLFW_MOUSE_BUTTON_LEFT))
	//if (InputHandler::GetCursorState())
	{
		_yaw += xoffset;
		_pitch += yoffset;

		if (_pitch > 89.0f)
			_pitch = 89.0f;
		else if (_pitch < -89.0f)
			_pitch = -89.0f;

		_viewDirection = glm::vec3
		(
			cos(glm::radians(_yaw)) * cos(glm::radians(_pitch)),
			-sin(glm::radians(_pitch)),
			sin(glm::radians(_yaw)) * cos(glm::radians(_pitch))
		);

		_viewDirection = glm::normalize(_viewDirection);
		
		_right = glm::normalize(glm::cross(_viewDirection, _up));
		
		_forward = _viewDirection;

	}

	_prevX = mx;
	_prevY = my;

}