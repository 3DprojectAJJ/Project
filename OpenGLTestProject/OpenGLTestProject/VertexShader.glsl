#version 430

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 vertexUV;
layout (location = 2) in vec3 vertexNormal;


out vec2 UV;

void main()
{
	gl_Position = vec4(vertexPosition, 1.0);
	UV = vec2(vertexUV.x, 1-vertexUV.y);
}