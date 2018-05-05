#version 330 core
//view space lighting

//object material
struct Material 
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_emissive1;
	float shininess;
};

//directional/point/spot light
struct Light 
{
	int active;
	int type;
	
	vec3 position;
	vec3 direction;

	float constant;
	float linear;
	float quadratic;

	float cutoff;
    float outercutoff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

#define NR_LIGHT 8
#define DIR_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

in vec4 _eyeposition;
in vec3 _eyenormal;
in vec2 _texcoord;

uniform Material material;
uniform mat4 view;
uniform Light lights[NR_LIGHT];

//indirect output to HDR frame buffer
layout(location = 0) out vec4 fragcolor;
layout(location = 1) out vec4 brightcolor;
//out vec4 color;

vec3 CalLight(Light light, vec3 fragNorm, vec3 fragPos, vec3 viewDir, vec2 texcoord)
{
	//light direction
	vec3 lightDir;

	vec3 lightEyePos = vec3(0.0);

	if(light.type == DIR_LIGHT)
	{
		lightDir = normalize(view * vec4(light.direction, 0.0)).xyz;
	}
	else
	{
		lightEyePos = (view * vec4(light.position, 1.0)).xyz;
		lightDir = normalize(lightEyePos - fragPos);
	}

	//diffuse
	float diffuseTerm = max(dot(fragNorm, lightDir), 0.0);
	
	//H(half vector) = (l + v) / ||l + v|| -> blinn-phong's specular
	vec3 H = normalize(lightDir + viewDir);
	float specularTerm = pow(max(dot(fragNorm, H), 0.0), material.shininess);

	vec3 ambient = light.ambient * texture(material.texture_diffuse1, texcoord).rgb;
	vec3 diffuse = light.diffuse * diffuseTerm * texture(material.texture_diffuse1, texcoord).rgb;
	vec3 specular = light.specular * specularTerm * texture(material.texture_specular1, texcoord).rgb;

	float distance = 0.0;
	//attenuation
	if(light.type != DIR_LIGHT)
	{
		distance = length(lightEyePos - fragPos); //Euclidean distance
		float attenuation = 1.0 / (light.constant + light.linear * distance + 
								light.quadratic * (distance * distance));

		if(light.type == SPOT_LIGHT)
		{
			vec3 eyeSpotDir = normalize(view * vec4(light.direction, 0.0)).xyz;

			float theta = dot(lightDir, -eyeSpotDir);
			float epsilon = light.cutoff - light.outercutoff;
			float intensity = clamp((theta - light.outercutoff) / epsilon, 0.0, 1.0);

			diffuse *= intensity;
			specular *= intensity;
		}

		ambient *= attenuation;
		diffuse *= attenuation;
		specular *= attenuation;
	}

	return (ambient + diffuse + specular);
}

void main()
{
	//equals to vec3(0,0,0) - myPos, camera position is always (0,0,0) (since we are in view space now)
	vec3 viewerDir = normalize(-_eyeposition.xyz);
	vec3 viewNorm = _eyenormal;			//normal in view space
	vec3 viewPos = _eyeposition.xyz;	//position in view space
	vec2 texcoord = _texcoord;

	vec3 result = vec3(0.0);

	for(int i = 0; i < NR_LIGHT; i++)
	{
		if(lights[i].active != 0)
		{
			result += CalLight(lights[i], viewNorm, viewPos, viewerDir, texcoord);
		}
	}

	//emissive texture do not effect by lights
	vec3 emission = texture(material.texture_emissive1, texcoord).rgb;

	//draw lighted pixel to where emissive map is black (0,0,0)
	if(emission != 0)
	{
		fragcolor = vec4(emission + texture(material.texture_diffuse1, texcoord).rgb, 1.0);
	}
	else
	{
		fragcolor = vec4(result, 1.0);
	}
	
	brightcolor = vec4(emission, 1.0);
}
