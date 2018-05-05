#pragma once

#include "Engine.h"

class Camera
{
public:
	Camera();
	virtual ~Camera() {};

	inline glm::vec3 GetEyePosition() { return _eyePosition; }
	inline glm::vec3 GetViewDirection() { return _viewDirection; }
	inline glm::vec3 GetForwardVector() { return _forward; }
	inline glm::vec3 GetRightVector() { return _right; }
	inline glm::vec3 GetUpVector() { return _up; }

	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjectionMatrix();

	virtual void Update(const GLfloat& deltaTime);
	virtual void Reset();

	virtual void SetupCamera();

protected:
	glm::mat4 _viewMatrix;
	glm::mat4 _projectionMatrix;

	glm::vec3 _eyePosition, _viewDirection;
	glm::vec3 _forward, _right, _up, _rise;

	GLdouble _prevX, _prevY;
	GLfloat _prevScrollX, _prevScrollY;

	GLfloat _fovy, _aspect;
	GLfloat _pitch, _yaw;
	

	GLboolean _flagFirstCallback;

	void HandleMouseMove(const GLfloat& deltaTime);
	void HandleKeyMovement(const GLfloat& deltaTime);
	void HandleMouseScroll(const GLfloat& deltaTime);
};

