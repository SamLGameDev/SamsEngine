#version 460 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT 
{
    vec3 Normal;
} In[];

layout (std140, binding = 0) uniform Transforms
{
     mat4 View; 
     mat4 Projection; 
};

const float Magnitude = 0.4;

void GenerateLine(vec4 Pos, vec3 Norm)
{
    gl_Position = Projection * Pos;
    EmitVertex();
    gl_Position = Projection * (Pos + vec4(Norm, 0) * Magnitude);
    EmitVertex();
    EndPrimitive();
}

void main()
{
   for (int i = 0; i < 3; i++)
   {
      GenerateLine(gl_in[i].gl_Position, In[i].Normal);   
   };
}