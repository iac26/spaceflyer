#version 410 core

out vec4 FragColor;
in vec3 texCoord;
uniform samplerCube Texture;

void main()
{
	

	FragColor = texture(Texture, texCoord);

} 