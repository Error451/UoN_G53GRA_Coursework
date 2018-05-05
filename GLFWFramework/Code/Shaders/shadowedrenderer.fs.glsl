#version 330 core
//simplified renderer, only receive single directional light

//object material
struct Material 
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

in VS_OUT{
	vec4 eyeposition;
	vec3 eyenormal;
	vec4 fragPosLightSpace;
} fs_in;

uniform Material material;
uniform sampler2D shadowMap;
uniform mat4 view;
uniform vec3 lightDir;

out vec4 fragcolor;

float ShadowCalculation(vec4 fragPosLightSpace)
{
	//convert to NDC, only make sure right answer will be returned
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	//map coords from [-1,1] to [0,1](depthmap's range)
	projCoords = projCoords * 0.5 + 0.5;

	//cloest depth map obtain from previous pass
	float cloestDepth = texture(shadowMap, projCoords.xy).r;
	//current frag's depth
	float currentDepth = projCoords.z;

	//bias value to eliminate shadow acne problem
	//float bias = max(0.05 * (1.0 - dot(fs_in.eyenormal, lightDir)), 0.005);
	float bias = 0.0;

	//original shadow
	//float shadow = (currentDepth - bias) > cloestDepth ? 1.0 : 0.0;

	//smoothed shadow - 3x3 mean filter
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -2; x <= 2; x++)
	{
		for(int y = -2; y <= 2; y++)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x,y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;

	return shadow;
}

void main()
{
	//simple view space blinn-phong's shading, no texture
	//light direction in view space - directional light only so not deducted by fragpos
	vec3 lightDir = normalize((view * vec4(lightDir, 0.0)).xyz);

	float diffuseMul = max(dot(fs_in.eyenormal, lightDir), 0.0);
	vec3 diffuse = material.diffuse * diffuseMul; //hardcoded object surface color

	vec3 ambient = material.ambient;

	//equals to float3(0,0,0) - myPos, camera position is float3(0,0,0) (since this is view space)
	vec3 viewerVector = normalize(-fs_in.eyeposition.xyz); 
	vec3 halfVector = normalize(lightDir + viewerVector);

	float specularMul = pow(max(dot(fs_in.eyenormal, halfVector), 0.0), material.shininess);
	vec3 specular = material.specular * specularMul;

	//calculate shadow
	float shadow = ShadowCalculation(fs_in.fragPosLightSpace);

	//color = vec4(diffuse + ambient + specular, 1.0);//vec4(_colour, 1.0f);	
	vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular));

	fragcolor = vec4(lighting, 1.0);//vec4(1-shadow, 1-shadow, 1-shadow, 1.0);//vec4(lighting, 1.0);
}