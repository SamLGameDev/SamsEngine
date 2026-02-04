#version 330 core
out vec4 FragColor;
in vec3 VertexColor;
in vec2 TexCoord; 
uniform sampler2D ourTexture0; 
uniform float Visibility; 
void main()
{
   FragColor = texture(ourTexture0, TexCoord) * vec4(VertexColor, Visibility);
}