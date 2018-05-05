#version 330 core

in vec4 _eyeposition;
in vec3 _eyenormal;

uniform mat4 view;

//out vec4 color;

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 brightcolor;

void main()
{
	//simple view space blinn-phong's shading, no texture
	vec3 lightDir = normalize(view * vec4(0.0, 0.0, 10.0, 1.0)).xyz;

	float diffuseMul = max(dot(_eyenormal, lightDir), 0.0);
	vec3 diffuse = vec3(0.5, 0.5, 0.5) * diffuseMul; //hardcoded object surface color

	vec3 ambient = vec3(0.1, 0.1, 0.1);

	//equals to float3(0,0,0) - myPos, camera position is float3(0,0,0) (since this is view space)
	vec3 viewerVector = normalize(-_eyeposition.xyz); 
	vec3 halfVector = normalize(lightDir + viewerVector);

	float specularMul = pow(max(dot(_eyenormal, halfVector), 0.0), 300);
	vec3 specular = vec3(1.0, 1.0, 1.0) * specularMul;

	color = vec4(diffuse + ambient + specular, 1.0);//vec4(_colour, 1.0f);	

	//greyscale intensity
	float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0f)
        brightcolor = vec4(color.rgb, 1.0);
    else
        brightcolor = vec4(0.0, 0.0, 0.0, 1.0);
}