#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexture;
layout (location = 3) in mat4 aInstanceMatrix;

out VS_OUT
{
   vec3 Normal;
   vec2 TexCoord; 
   vec3 FragPos;

} Out;

uniform mat4 Model; 
uniform mat3 NormalModel;

layout (std140, binding = 0) uniform Transforms
{
     mat4 View; 
     mat4 Projection; 
};

void main()
{

   mat4 model = mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        1, 1, 1, 1.0
    );;
   if (aInstanceMatrix == mat4(0))
   {
       model = mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        1, 1, 1, 1.0
    );
   }

   gl_Position = Projection * View *  aInstanceMatrix * vec4(aPos, 1.0);
   Out.FragPos = vec3((View * aInstanceMatrix * vec4(aPos, 1.0)));
   Out.Normal = NormalModel * aNormal;
   Out.TexCoord = aTexture;
}