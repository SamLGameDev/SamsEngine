#version 460 core
layout (points) in;
layout (points, max_vertices = 1) out;
void main()
{
   for (int i = 0; i < 1; i++)
   {
        gl_Position = gl_in[i].gl_Position;
	     EmitVertex();
	};
   EndPrimitive();
}