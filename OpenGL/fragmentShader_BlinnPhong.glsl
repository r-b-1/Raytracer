#version 330 core

layout(location=0) out vec4 fragmentColor;

uniform vec3 diffuseComponent;
uniform vec3 specularComponent;
uniform float phongExponent;

uniform vec4 eyePosWorld;

in vec4 normal;
in vec4 lightDir;
in vec4 vertexWorldPos;

void main(void)
{
  vec3 N = normalize(normal.xyz);
  vec3 L = normalize(lightDir.xyz);

  // Lambertian term: no light contribution on the back side of the surface.
  float nDotL = max(0.0, dot(N, L));
  vec3 diffuseShading = diffuseComponent * nDotL;

  // Blinn-Phong term: the half vector sits between the light and the eye.
  vec3 V = normalize(eyePosWorld.xyz - vertexWorldPos.xyz);
  vec3 H = normalize(L + V);
  float specularAmount = pow(max(0.0, dot(N, H)), phongExponent);
  vec3 specularShading = specularComponent * specularAmount;

  // Suppress the highlight on the unlit side, where nDotL is zero.
  vec3 color = (nDotL > 0.0) ? (diffuseShading + specularShading) : vec3(0.0);

  fragmentColor = vec4(color, 1.0);
}
