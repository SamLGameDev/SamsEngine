#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexture;
layout (location = 3) in mat4 aInstanceMatrix;

out VS_OUT
{
   vec3 TexCoords; 

} Out;

layout (std140, binding = 0) uniform Transforms
{
     mat4 View; 
     mat4 Projection; 
};

void main()
{
    Out.TexCoords = aPos;
    vec4 Pos = Projection * mat4(mat3(View)) * vec4(aPos, 1.0);
    gl_Position = Pos.xyww;
}  