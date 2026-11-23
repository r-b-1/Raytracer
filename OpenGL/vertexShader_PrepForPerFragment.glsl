#version 330 core

layout(location=0) in vec3 in_Position;
layout(location=1) in vec3 in_Normal;

uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 normalMatrix;

uniform vec4 lightPosWorld;

out vec4 normal;
out vec4 lightDir;
out vec4 vertexWorldPos;

void main(void)
{
  // Normalize only xyz: the 4x4 normal matrix carries a translation row, so
  // normalizing the whole vec4 would shrink the direction for objects that are
  // offset from the origin (e.g. the sun), making them render nearly black.
  normal = vec4(normalize((normalMatrix * vec4(in_Normal, 0.0)).xyz), 0.0);

  vertexWorldPos = modelMatrix * vec4(in_Position, 1.0);
  lightDir = normalize(lightPosWorld - vertexWorldPos);

  gl_Position = projMatrix * viewMatrix * vertexWorldPos;
}
