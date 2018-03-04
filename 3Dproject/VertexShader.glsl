#version 430
layout(location = 0) in vec3 vertexPositionModelSpace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal;

layout(location = 3) in vec3 vertexAmbient;
layout(location = 4) in vec3 vertexDiffuse;
layout(location = 5) in vec4 vertexSpecular;

out vec2 geometryUV;
out vec3 geometryNormal;
out vec3 geometryAmbient;
out vec3 geometryDiffuse;
out vec4 geometrySpecular;

void main()
{
	gl_Position = vec4(vertexPositionModelSpace, 1.0);
	geometryUV = vertexUV;
	geometryNormal = vertexNormal;
}