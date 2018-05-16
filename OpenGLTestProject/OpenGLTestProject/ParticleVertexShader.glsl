#version 430

layout (location = 10) in vec4 vertexPosition;

void main()
{
	gl_Position = vertexPosition;
}