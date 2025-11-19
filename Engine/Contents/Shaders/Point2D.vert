#version 330 core
layout (location = 0) in vec2 aPos;

void main()
{
   vec4 Pos = vec4(aPos, 0, 1.0);
   gl_Position = Pos;
}