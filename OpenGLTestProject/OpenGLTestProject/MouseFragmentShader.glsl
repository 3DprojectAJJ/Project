#version 430

uniform float index;

layout(location = 0) out vec3 color;

void main()
{
	color = vec3(index, 0, 0);
}