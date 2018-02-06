#version 430

in vec3 fragmentColor;

layout(location = 0) out vec3 color;

void main()
{
  color = fragmentColor;
}