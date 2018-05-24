#version 430
layout (location = 0) in vec3 vertexPosistion;

uniform mat4 model;

void main()
{
    gl_Position = model * vec4(vertexPosistion, 1.0);
}  