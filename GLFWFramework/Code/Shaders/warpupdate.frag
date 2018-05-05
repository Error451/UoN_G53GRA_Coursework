#version 420 core

//interpolated by rasterizer
in vec2 _texcoord;

uniform vec2 texscale;
uniform vec3 emitterPos;
uniform vec3 emitterSize;
uniform vec4 direction;
uniform vec2 speedParam;
uniform vec4 config;
uniform float time;
uniform int isInit;
uniform sampler2D particlesBuf;

//color -> new position
layout(location = 0) out vec4 color;

float nrand(vec2 uv, float salt)
{
	uv += vec2(salt, config.z);
	return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
}

vec4 newParticle(vec2 uv)
{
	//random position
	vec3 p = vec3(nrand(uv, time + 1), nrand(uv, time + 2), nrand(uv, time + 3));
	p = (p - vec3(0.5)) * emitterSize + emitterPos;

	//life time
	float l = abs(config.y * (0.5 + nrand(uv, time + 0)));

	//Throttle : discard particle emission by adding offset
	vec4 offs;
	if(uv.x > config.x)
	{
		offs = vec4(1e8, 1e8, 1e8, -1e8);
	}
	else
	{
		offs = vec4(0.0);
	}

	return (vec4(p, l) + offs);
}

//position dependant velocity field
vec3 get_velocity(vec3 p, vec2 uv)
{
	// Random vector.
	vec3 v = vec3(nrand(uv, 4), nrand(uv, 5), nrand(uv, 6));
	v = (v - vec3(0.5)) * 2;

	// Apply the spread parameter.
	v = mix(direction.xyz, v, direction.w);

	// Apply the speed parameter.
	v = normalize(v) * mix(speedParam.x, speedParam.y, nrand(uv, 7));

	return v;
}

void main()
{
	
	vec2 tex = _texcoord * texscale;
	//vec2 tex = vec2(_texcoord.x * 6.25, _texcoord.y * 4.6875);

	if(isInit == 0)
	{
		vec4 p = texture2D(particlesBuf, tex);
		
		//update if particle is still alive, otherwise reset it
		if(p.w > 0)
		{
			float dt = config.w;
			p.xyz += get_velocity(p.xyz, tex) * dt; //position
			p.w = p.w - dt;  //dt;
			color = p;
		}
		else
		{
			color = newParticle(tex);
		}
	}
	else
	{
		color = newParticle(tex);
	}
}