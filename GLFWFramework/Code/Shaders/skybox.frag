#version 330 core

in vec3 texcoords;

uniform samplerCube cubemap;

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 brightness;

void main()
{
	//glsl intrinsic function
	color = texture(cubemap, texcoords);
	brightness = vec4(0.0);
}