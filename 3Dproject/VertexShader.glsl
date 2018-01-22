#version 430
layout(location = 0) in vec3 vertexPositionModelSpace;
uniform mat4 MVP;
void main()
{
gl_Position = MVP*vec4(vertexPositionModelSpace, 1.0);
}