#version 330 core

in vec2 _texcoord;

uniform sampler2D depthMap;
uniform float znear;
uniform float zfar;

out vec4 FragColor;

//required when using a perspective projection matrix
float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0; //back to NDC
	return (2.0 * znear * zfar) / (zfar + znear - z * (zfar - znear));
}

void main()
{
	float depthValue = texture(depthMap, _texcoord).r;
	//FragColor = vec4(vec3(LinearizeDepth(depthValue) / zfar), 1.0); //perspective
	FragColor = vec4(vec3(depthValue), 1.0); //orthographic
}
