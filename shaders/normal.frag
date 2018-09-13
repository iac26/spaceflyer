#version 410 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;
in vec2 texCoord;
in vec3 normal;
in vec3 position;

uniform float ambientIntensity;
uniform vec3 ambientColor;

uniform float diffuseIntensity;
uniform vec3 diffuseColor;

uniform float specularIntensity;
uniform vec3 specularColor;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform sampler2D Texture;
uniform sampler2D Lightmap;
uniform sampler2D Reflectmap;
uniform sampler2D JustLight;
uniform samplerCube Skybox;
uniform vec3 color1, color2;

void main()
{

	vec3 color = texture(Texture, texCoord).rgb;

	vec3 norm = normalize(normal);
	vec3 lightDir = normalize(lightPos - position);
	vec3 viewDir = normalize(viewPos - position);
	vec3 reflectDir = reflect(-lightDir, norm);

	vec3 I = normalize(position - viewPos);
    vec3 R = reflect(I, normalize(normal));
    vec3 refl= texture(Skybox, R).rgb*texture(Reflectmap, texCoord).rgb;

    vec3 addition = texture(Lightmap, texCoord).rgb;
	vec3 ambient = ambientIntensity*ambientColor + addition;
	float brightness = dot(addition, vec3(0.2126, 0.7152, 0.0722));
   

	vec3 diffuse = max(dot(norm, lightDir), 0.0)*diffuseIntensity*diffuseColor;

	float spec = pow(max(dot(viewDir, reflectDir),0.0),32);
	vec3 specular = spec * specularIntensity * specularColor;

	vec3 light = texture(JustLight, texCoord).rgb;
	float test = dot(light, vec3(0.2126, 0.7152, 0.0722));

	if(test < 0.5) {
		FragColor = vec4((ambient + diffuse + specular + refl)*color, 1.0);
	} else {
		FragColor = vec4(0.0, 0.0, 0.0, 0.0);
	}
	
	
	if(brightness > 0.5)
        BrightColor = vec4(color, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);





} 