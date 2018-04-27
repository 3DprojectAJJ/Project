#version 430

layout (location = 0) in vec3 vertexPosition;
layout (location = 1) in vec2 vertexUV;
//layout (location = 2) in vec3 vertexNormal;

layout (location = 3) in vec3 vertexAmbient;
layout (location = 4) in vec3 vertexDiffuse;
layout (location = 5) in vec3 vertexSpecular;
layout (location = 6) in int vertexSpecularExponent;


out vec2 UV;
out vec3 a;
out vec3 d;
out vec3 s;
out int e;
void main()
{
	gl_Position = vec4(vertexPosition, 1.0);
	UV = vec2(vertexUV.x, 1-vertexUV.y);
	a = vertexAmbient;
	d = vertexDiffuse;
	s = vertexSpecular;
	e = vertexSpecularExponent;
}