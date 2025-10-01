#version 330 core
out vec4 FragColor;

in GS_OUT
{
   vec3 TexCoords; 
} In;

uniform samplerCube Map;

void main()
{    
    FragColor = texture(Map, In.TexCoords);
}