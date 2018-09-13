#version 410 core

out vec4 FragColor;
in vec2 texCoord;
uniform int shake;
uniform float time;
uniform float speed;
uniform sampler2D Texture;
uniform sampler2D Blur;

void main()
{
	vec2 offset = vec2(0,0);

	if(shake == 1) {
		offset = vec2(speed*0.0005*sin(time), speed*0.0005*cos(time));
	}

	vec3 color = texture(Texture, texCoord+offset).rgb;      
    vec3 bloomColor = texture(Blur, texCoord+offset).rgb;
    vec3 hdrColor = color+bloomColor;
    vec3 result = vec3(1.0) - exp(-hdrColor * 1.5);
    //result = pow(result, vec3(1.0 / 1.5)); //GAMMA
	FragColor = vec4(result, 1.0);




} 