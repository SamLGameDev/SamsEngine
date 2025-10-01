#version 460 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT
{
    vec3 VertexColor; 
} In[];

out GS_OUT
{
    vec3 VertexColor; 
} Out;

void main()
{
   for (int i = 0; i < 3; i++)
   {
        gl_Position = gl_in[i].gl_Position;
        Out.VertexColor = In[i].VertexColor;
        EmitVertex();
   };
   EndPrimitive();
}