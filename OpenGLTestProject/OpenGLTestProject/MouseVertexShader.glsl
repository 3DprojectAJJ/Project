#version 430

layout (location = 0) in vec3 vertexPosition;

uniform mat4 model;
uniform mat4 mview;
uniform mat4 mprojection;

void main()
{
	gl_Position = mprojection*mview*model* vec4(vertexPosition, 1.0f);
}