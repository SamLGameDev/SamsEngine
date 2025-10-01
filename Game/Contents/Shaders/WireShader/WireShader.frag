#version 330 core
out vec4 FragColor;

in GS_OUT
{
   vec3 VertexColor;
} In;

void main()
{
   FragColor = vec4(In.VertexColor, 1);
}