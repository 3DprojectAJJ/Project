#version 430
layout(location = 0) in vec3 vertexPositionModelSpace;
layout(location = 1) in vec3 vertexColor;
uniform mat4 MVP;

out vec3 fragmentColor;

void main()
{
gl_Position = MVP*vec4(vertexPositionModelSpace.x,-vertexPositionModelSpace.y,vertexPositionModelSpace.z, 1.0);
fragmentColor = vertexColor;
}