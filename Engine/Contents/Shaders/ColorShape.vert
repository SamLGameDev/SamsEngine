#version 450


layout(location = 0) in vec2 aPos;

layout(location = 0) out vec3 fragColors;


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
    gl_Position = Projection * View * Model * vec4(aPos, 0, 1.0);
    fragColors = Color;
}
