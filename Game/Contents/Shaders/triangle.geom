#version 460 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

layout(location = 0) in vec3 fragColors[];
layout(location = 0) out vec3 fragColor;

void main()
{
   for (int i = 0; i < 3; i++)
   {
        gl_Position = gl_in[i].gl_Position;
		fragColor = fragColors[i];
	    EmitVertex();
	};
   EndPrimitive();
}
