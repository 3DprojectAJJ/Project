#version 430
layout(location = 0) in vec3 vertexPositionModelSpace;
layout(location = 1) in vec2 vertexUV;

out vec2 geometryUV;

void main()
{
	gl_Position = vec4(vertexPositionModelSpace, 1.0);
	geometryUV = vertexUV;
}