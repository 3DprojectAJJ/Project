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
	fragColor = inColor.xyz;

	vec4 eyePos = model * pView * vec4(vertexPosition, 1.0f);	  //transform to eye-space
    eyePos.xy += particleSize * (corner - vec2(0.5)); //add corner position
	fragPosition = model * vec4(vertexPosition, 1.0f);
	gl_Position = pProjection * eyePos;
}