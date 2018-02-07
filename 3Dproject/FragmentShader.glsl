#version 430

in vec3 fragmentColor;
in vec3 fragmentNormal;
in vec3 fragmentPosition;

layout(location = 0) out vec3 color;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 position;

void main()
{
  color = fragmentColor;
  normal = fragmentNormal;
  position = fragmentPosition;
}