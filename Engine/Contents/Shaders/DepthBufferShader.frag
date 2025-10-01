#version 330 core
out vec4 FragColor;

float near = 0.1;
float far = 100.0;

float LinearizeDepth(float Depth)
{
    float ndc = Depth * 2 - 1;
    float depth = (2 * near * far) / (far + near - ndc * (far - near));
    return depth;
}

void main()
{
    float z = LinearizeDepth(gl_FragCoord.z) / far;
    FragColor = vec4(vec3(z), 1.0);
}