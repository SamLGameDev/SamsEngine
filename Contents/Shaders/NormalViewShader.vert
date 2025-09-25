#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT
{
   vec3 Normal;
} Out;

layout (std140, binding = 0) uniform Transforms
{
     mat4 View; 
     mat4 Projection; 
};

uniform mat4 Model; 
uniform mat3 NormalModel;

void main()
{
   gl_Position = View * Model * vec4(aPos, 1.0);;
   Out.Normal = normalize(vec3(vec4(NormalModel * aNormal, 0)));
}