#version 460 core
layout (location = 0) in vec2 aPos;

uniform mat4 Model; 
layout (std140, binding = 0) uniform Transforms
{
     mat4 View; 
     mat4 Projection; 
};
void main()
{
   gl_Position = vec4(aPos, 0, 1);
   
}
