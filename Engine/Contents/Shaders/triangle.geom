#version 460 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

layout(location = 0) in vec3 fragColors[];
layout(location = 1) in vec2 TexCoord[];
layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 aTexCoord;

void main()
{
   for (int i = 0; i < 3; i++)
   {
        gl_Position = gl_in[i].gl_Position;
		fragColor = fragColors[i];
		aTexCoord = TexCoord[i];
	    EmitVertex();
	};
   EndPrimitive();
}
