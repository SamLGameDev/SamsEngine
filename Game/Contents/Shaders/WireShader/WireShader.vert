#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out VS_OUT
{
   vec3 VertexColor;
} Out;

uniform mat4 Model; 

layout (std140, binding = 0) uniform Transforms
{
     mat4 View; 
     mat4 Projection; 
};

void main()
{
   gl_Position =  Projection * View * Model * vec4(aPos, 1.0);;
   Out.VertexColor = aColor;
}