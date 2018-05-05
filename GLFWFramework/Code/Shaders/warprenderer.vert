#version 330 core

// Vertex format:
// position.x  = texture switch (0/1)
// texcoord.xy = uv for ParticleTex
//
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texcoord;

//buffer with last state
uniform sampler2D ptlBuf1;
//current state buffer
uniform sampler2D ptlBuf2;

uniform vec2 texelSize;
uniform vec4 ptlColor;
uniform float ptlTail;

uniform mat4 view;
uniform mat4 projection;

out vec4 _color;

void main()
{
	vec2 uv = texcoord.xy + texelSize.xy / 2;

	vec4 p1 = textureLod(ptlBuf1, uv, 0);
	vec4 p2 = textureLod(ptlBuf2, uv, 0);
	float sw = position.x;

	vec4 pos;
	if(p1.w < 0)
	{
		//particle position is already world coordinate based on emitter's position
		pos = projection * view * vec4(p2.xyz, 1);
	}
	else
	{
		//principle of intrinsic function mix -> mix(x, y, a) <=> x * (1 - a) + y * a
		vec3 p = mix(p2.xyz, p1.xyz, (1.0 - sw) * ptlTail);
		pos = projection * view * vec4(p, 1);
	}

	gl_Position = pos;

	_color = vec4(ptlColor.rgb * normalize(pos).xyz + 0.8, 0.6);
	//set the tail to transparent
	_color.a *= sw;
}
