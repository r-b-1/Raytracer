#version 330 core

layout(location=0) out vec4 fragmentColor;

uniform vec3 diffuseComponent;

in vec4 normal;
in vec4 lightDir;

void main(void)
{
  // The interpolated normals and light directions are no longer unit length,
  // so normalize them per fragment before shading. Use xyz only; the w
  // component of the interpolated vec4 is not part of the direction.
  float val = max(0.0, dot(normalize(normal.xyz), normalize(lightDir.xyz)));
  vec3 diffuseShading = diffuseComponent * val;

  fragmentColor = vec4(diffuseShading, 1.0);
}
