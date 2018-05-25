#version 430
layout (location = 0) in vec3 vertexPosition;

uniform mat4 model;
uniform mat4 lightSpaceMat;

void main()
{
    gl_Position = lightSpaceMat * model * vec4(vertexPosition, 1.0);
}  