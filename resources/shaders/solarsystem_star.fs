#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec2 TexCoord;
in vec3 FragPos;  

// texture samplers
uniform sampler2D texture1;

void main()
{
    vec3 result = texture(texture1, TexCoord).rgb;
    FragColor = vec4(result, 1.0);
} 
