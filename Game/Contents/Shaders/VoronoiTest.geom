#version 450 core
layout (points) in;
layout (triangle_strip, max_vertices = 146) out;

layout(location = 0) in uint Instance[];
layout(location = 0) out vec3 fragColor;

struct Tri
{
    vec3 Verts[3];
};

struct FaceTri
{
    Tri Tris[6];
    uint NumTris;
};

struct CellTri
{
    FaceTri Faces[20];
    uint NumFaces;
};

layout (std140, binding = 0) uniform Transforms
{
     mat4 View; 
     mat4 Projection; 
};

layout (std140, binding = 1) uniform InstanceTransform
{
	mat4 Model;
	vec3 Color;
};

layout (std140, binding = 2) uniform VoronoiTransforms
{
	mat4 Models[100];
	vec3 Colors[100];
};


layout(std430, binding = 3) buffer VoronoiSSBOOut {
    uint NumCells;
	uint DebugNum;
    CellTri CutCells[10];

};



void main()
{
	CellTri cell = CutCells[Instance[0]];
	uint numverts = 0;
    for (uint i = 0; i < cell.NumFaces; i++)
    {
		for (uint j = 0; j < cell.Faces[i].NumTris; j++)
		{
			Tri newTri = cell.Faces[i].Tris[j];
			
			for (uint t = 0 ; t < 3; t++)
			{
				gl_Position = Projection * View * Models[Instance[0]] * vec4(newTri.Verts[t], 1);
				fragColor = Colors[Instance[0]];
				EmitVertex();
				numverts++;
			};
			EndPrimitive();
		};
	};
	if (numverts > DebugNum) DebugNum = numverts;
}
