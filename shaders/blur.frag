
#version 410 core
out vec4 FragColor;
  
in vec2 texCoord;

uniform sampler2D image;
  
uniform bool horizontal;
uniform float speed;
uniform float weight[5] = float[] (0.4, 0.3, 0.2, 0.1, 0.05);

void main()
{     
    float spd = min(speed, 2.0);
    vec2 tex_offset = 4.0 / textureSize(image, 0);
    vec3 result = texture(image, texCoord).rgb * weight[0]*(spd/4+0.5);
    if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(image, texCoord + vec2(tex_offset.x * i, 0.0)).rgb * weight[i]*(spd/4+0.5);
            result += texture(image, texCoord - vec2(tex_offset.x * i, 0.0)).rgb * weight[i]*(spd/4+0.5);
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(image, texCoord + vec2(0.0, tex_offset.y * i)).rgb * weight[i]*(spd/4+0.5);
            result += texture(image, texCoord - vec2(0.0, tex_offset.y * i)).rgb * weight[i]*(spd/4+0.5);
        }
    }
    FragColor = vec4(result, 1.0);
}