#version 330 core

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    float shininess;
};

out vec4 FragColor;
in vec3 VertexColor;
in vec2 TexCoord; 
uniform Material material;
uniform float Visibility; 
void main()
{
   FragColor = texture(material.texture_diffuse1, TexCoord);
}