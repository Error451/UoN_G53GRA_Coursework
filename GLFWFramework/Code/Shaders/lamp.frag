#version 330 core

uniform vec3 bulbColor;

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 brightness;
void main()
{
	color = vec4(bulbColor, 1.0);
	brightness = vec4(bulbColor, 1.0);
}