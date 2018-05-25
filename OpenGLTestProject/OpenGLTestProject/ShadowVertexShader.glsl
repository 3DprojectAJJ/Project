#version 430
layout (location = 0) in vec3 vertexPosistion;

uniform mat4 model;
uniform mat4 lightSpaceMat;

void main()
{
    gl_Position = lightSpaceMat * model * vec4(vertexPosistion, 1.0);
}  