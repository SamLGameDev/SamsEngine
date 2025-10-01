#version 460 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec3 Normal;
    vec2 TexCoord; 
    vec3 FragPos;
} In[];

out GS_OUT {
    vec3 Normal;
    vec2 TexCoord; 
    vec3 FragPos;
} Out;


layout (std140, binding = 1) uniform Time
{
     float time; 
};

vec4 Explode(vec4 Pos, vec3 Norm)
{
    vec3 dir = Norm * (time / 100) * 10;
    return Pos + vec4(dir, 0);
}

vec3 GetNormal()
{
   vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
   return normalize(cross(a, b));
}  

void main()
{
   vec3 normal = GetNormal();
   for (int i = 0; i < 3; i++)
   {
        gl_Position = gl_in[i].gl_Position;

        Out.Normal = In[i].Normal;
        Out.TexCoord = In[i].TexCoord;
        Out.FragPos = In[i].FragPos;

	EmitVertex();
   };
   EndPrimitive();
}