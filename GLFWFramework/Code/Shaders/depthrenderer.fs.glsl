#version 330 core

#define BIAS 0.048

void main()
{
	gl_FragDepth = gl_FragCoord.z;
	gl_FragDepth += gl_FrontFacing ? BIAS : 0.0;
}