/* Shader used only for texture scene,
*only support one directional light for simplicity
*/

#version 330 core
//simplified renderer, only receive single directional light

//object material
struct Material 
{
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_normal1;
	float shininess;
};

in VS_OUT{
	vec4 eyeposition;
	vec3 eyenormal;
	vec2 texcoord;
} fs_in;

uniform Material material;
uniform mat4 view;
uniform mat4 model;
uniform vec3 lightDir;

//k_a, k_d, k_s - coefficient for ambient/diffuse/specular respectively
uniform vec3 lightParams;

out vec4 fragcolor;

void main()
{
	//simple view space blinn-phong's shading, no texture
	//light direction in view space - directional light only so not deducted by fragpos
	vec3 lightDir = normalize((view * vec4(lightDir, 0.0)).xyz);

	vec3 diffuseColor = texture(material.texture_diffuse1, fs_in.texcoord).rgb;
	vec3 specularColor = texture(material.texture_specular1, fs_in.texcoord).rgb;
	//obtain normal and map it from [0,1] to [-1,1]
	vec3 normal = normalize(
		texture(material.texture_normal1, fs_in.texcoord).rgb * 2.0 - 1.0
		);

	// transform normal vector to range [-1,1] and transfer to view space
	normal = normalize((transpose(inverse(view * model)) * vec4(normal, 0.0)).xyz);

	//"normal" normal
	//normal = fs_in.eyenormal;

	float diffuseMul = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = lightParams.y * diffuseColor * diffuseMul; //hardcoded object surface color

	vec3 ambient = lightParams.x * diffuseColor;

	//equals to float3(0,0,0) - myPos, camera position is float3(0,0,0) (since this is view space)
	vec3 viewerVector = normalize(-fs_in.eyeposition.xyz); 
	vec3 halfVector = normalize(lightDir + viewerVector);

	float specularMul = pow(max(dot(normal, halfVector), 0.0), material.shininess);
	vec3 specular = lightParams.z * specularColor * specularMul;

	fragcolor = vec4(ambient + diffuse + specular, 1.0); //vec4(normal, 1.0); //vec4(diffuse + specular, 1.0);
}