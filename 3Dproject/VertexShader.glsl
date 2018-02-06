#version 430
layout(location = 0) in vec3 vertexPositionModelSpace;
layout(location = 1) in vec3 vertexColor;

out vec3 geometryColor;

void main()
{
	gl_Position = vec4(vertexPositionModelSpace, 1.0);
	geometryColor = vertexColor;
}