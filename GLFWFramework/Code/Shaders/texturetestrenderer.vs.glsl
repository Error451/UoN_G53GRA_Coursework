/* Shader used only for texture scene,
*only support one directional light for simplicity
*/
#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texcoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

//normal will be provided from normal map
out VS_OUT{
	vec4 eyeposition;
	vec3 eyenormal;
	vec2 texcoord;
} vs_out;

void main()
{
	vec4 _eyepos = view * model * vec4(position, 1.0);
	
	gl_Position = projection * _eyepos;

	vs_out.eyeposition = _eyepos;
	vs_out.eyenormal = normalize((transpose(inverse(view * model)) 
						* vec4(normal, 0.0)).xyz);
	vs_out.texcoord = texcoord;
}