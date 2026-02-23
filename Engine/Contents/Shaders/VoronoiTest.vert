#version 450

layout(location = 0) out uint Instance;


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

void main() {

    gl_Position = vec4(0 ,0 ,0, 1);
	Instance = gl_InstanceID;
}
