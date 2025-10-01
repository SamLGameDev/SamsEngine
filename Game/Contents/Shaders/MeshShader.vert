#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexture;
out vec3 VertexColor;
out vec4 Pos; 
out vec2 TexCoord; 
uniform float offset; 
uniform mat4 Transform; 
uniform mat4 Model; 
uniform mat4 View; 
uniform mat4 Projection; 
void main()
{
   Pos = Projection * View * Model * Transform * vec4(aPos, 1.0);
   gl_Position = Pos;
	TexCoord = aTexture;
}