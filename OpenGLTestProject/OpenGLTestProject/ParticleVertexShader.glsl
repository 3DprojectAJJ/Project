#version 430

layout (location = 10) in vec2 corner; 
layout (location = 11) in vec3 vertexPosition;
layout(location = 12) in vec4 inColor;

out vec3 fragColor;
out vec4 fragPosition;

uniform float particleSize;
uniform mat4 model;
uniform mat4 pView;
uniform mat4 pProjection;

void main()
{
	vec3 cameraRight = vec3(pView[0][0], pView[1][0], pView[2][0]);
	vec3 cameraUp = vec3(pView[0][1], pView[1][1], pView[2][1]);

    vec3 particlePosition = vertexPosition + cameraRight * corner.x * particleSize + cameraUp * corner.y * particleSize;

	fragPosition = pView * pProjection * vec4(particlePosition, 1.0f);
	fragColor = inColor.xyz;
}