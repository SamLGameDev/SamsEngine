#version 450


layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoord;

layout(location = 0) out vec3 fragColors;
layout(location = 1) out vec2 TexCoord;


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

void main() {
    gl_Position = Projection * View * Model * vec4(aPos, 1.0);
    fragColors = Color;
	TexCoord = aTexCoord;
}
