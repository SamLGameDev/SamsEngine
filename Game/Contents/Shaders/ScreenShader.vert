#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexture;
 
out VS_OUT
{
    vec2 TexCoord; 
} Out;

void main()
{
   gl_Position = vec4(aPos.x, aPos.y, 0, 1);
   Out.TexCoord = aTexture;
}