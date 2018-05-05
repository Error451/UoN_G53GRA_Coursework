#pragma once

#define MY_PI 3.1415926f


//My rotation based on Rodrigues rotation formula, 
//because the glm built-in one is not work (angle is not correct).
inline mat4 myRotate(mat4 modelIn, const float degrees, const vec3& axis) {

	float rad = (degrees * MY_PI) / 180;

	float sinD = sin(rad);
	float cosD = cos(rad);

	vec3 a = axis;
	mat3 rotMul1 = cosD * mat3(1.0f);
	mat3 rotMul2 = (1 - cosD) * mat3(a.x*a.x, a.x*a.y, a.x*a.z,
		a.x*a.y, a.y*a.y, a.y*a.z,
		a.x*a.z, a.y*a.z, a.z*a.z);
	mat3 rotMul3 = sinD * mat3(0, a.z, -a.y,
		-a.z, 0, a.x,
		a.y, -a.x, 0);

	return (modelIn * mat4(rotMul1 + rotMul2 + rotMul3));
}
