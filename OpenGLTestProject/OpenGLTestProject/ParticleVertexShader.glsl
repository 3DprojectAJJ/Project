#version 430

layout (location = 10) in vec3 vertexPosition;
layout (location = 11) in vec2 corner; 

uniform float particleSize;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 fragUV;

void main()
{
	vec4 eyePos = model * view * vec4(vertexPosition, 1.0f);
    gl_Position = projection * eyePos;
}