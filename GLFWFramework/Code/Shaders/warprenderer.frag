#version 330 core

in vec4 _color;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 brightColor;

void main()
{
	fragColor = _color;

    brightColor = vec4(fragColor.rgb, 1.0);
}