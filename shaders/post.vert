
#version 410 core
layout (location = 0) in vec3 pos;
layout (location = 2) in vec2 tex;

out vec2 texCoord;

void main()
{
	texCoord = tex;
    gl_Position = vec4(pos, 1.0);


}