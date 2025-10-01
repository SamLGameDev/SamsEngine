#version 460 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT
{
    vec2 TexCoord; 
} In[];

out GS_OUT
{
    vec2 TexCoord; 
} Out;

void main()
{
   for (int i = 0; i < 3; i++)
   {
        gl_Position = gl_in[i].gl_Position;
        Out.TexCoord = In[i].TexCoord;
	EmitVertex();
   };
   EndPrimitive();
}