#version 330 core

in vec2 _texcoord;

uniform sampler2D scene;
uniform sampler2D bloom;
uniform float exposure;

out vec4 color;

void main()
{
	vec3 hdrColor = texture(scene, _texcoord).rgb;
	vec3 bloomColor = texture(bloom, _texcoord).rgb;

	hdrColor += bloomColor; //additive blending

	//reinhard tone mapping - temprorily deprecated - hard to determined proper parameters
	vec3 result = hdrColor; //vec3(1.0) - exp(-hdrColor * exposure);

	color = vec4(result, 1.0);
}
