#version 330 core

layout(location=0) out vec4 fragmentColor;

in vec4 normal;

void main(void)
{
  // Show the world-space normal as a color: [-1, 1] mapped to [0, 1].
  vec3 n = normalize(normal.xyz);
  fragmentColor = vec4(n * 0.5 + 0.5, 1.0);
}
