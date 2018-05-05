#version 330 core

#define PI 3.14159265

in vec2 _texcoord;

uniform sampler2D texture;
uniform float resolution;
uniform float radius;
uniform vec2 dir; //smooth direction x or y
uniform float sigma;

//out vec4 color;

layout(location = 0) out vec4 color;

//gaussian
float gaussian(int x)
{
	return (1 / (sigma * sqrt(2 * PI)) * exp(-0.5f * pow((x / sigma), 2)));
}

void main()
{
	//final sum
	vec4 sum = vec4(0.0);

	//original texture coordinates
	vec2 tc = _texcoord;

	//the amount to blur, i.e. how far off center to sample from 
	//1.0 -> blur by one pixel
	//2.0 -> blur by two pixels, etc.
	float blurScale = radius / resolution;

	vec2 tcOffset = (1.0 / textureSize(texture, 0)) * blurScale;

	float w1 = gaussian(1);
	float w2 = gaussian(2);
	float w3 = gaussian(3);
	float w4 = gaussian(4);

	//apply blurring, using a 9-tap filter : 9x9
    //unrolled loop for efficiency
	sum += texture2D(texture, tc - 4.0 * tcOffset * dir) * w4;
	sum += texture2D(texture, tc - 3.0 * tcOffset * dir) * w3;
	sum += texture2D(texture, tc - 2.0 * tcOffset * dir) * w2;
	sum += texture2D(texture, tc - 1.0 * tcOffset * dir) * w1;

	sum += texture2D(texture, tc) * gaussian(0);

	sum += texture2D(texture, tc + 1.0 * tcOffset * dir) * w1;
	sum += texture2D(texture, tc + 2.0 * tcOffset * dir) * w2;
	sum += texture2D(texture, tc + 3.0 * tcOffset * dir) * w3;
	sum += texture2D(texture, tc + 4.0 * tcOffset * dir) * w4;

	color = vec4(sum.rgb, 1.0);
}
