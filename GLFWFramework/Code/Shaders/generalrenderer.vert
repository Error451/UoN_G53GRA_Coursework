#version 330 core

//layout (location = 0) in vec3 position;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 _eyeposition;
out vec3 _eyenormal;

void main()
{
	//vec4 _position = vec4(position, 1.0f);
	//gl_Position = projection * view * model * _position;

	//_colour = position + 0.5f;

	vec4 _eyepos = view * model * vec4(position, 1.0f);
	gl_Position = projection * _eyepos;

	_eyeposition = _eyepos;
	_eyenormal = normalize((transpose(inverse(view * model)) * vec4(normal, 0.0)).xyz);
}

