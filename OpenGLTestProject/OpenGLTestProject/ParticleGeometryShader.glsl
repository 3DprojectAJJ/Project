#version 430

layout (points) in;
layout (triangle_strip, max_vertices = 6) out;

in vec2 UV[];
in vec3 a[];
in vec3 diff[];
in vec4 s[];

out vec2 fragUV;
out vec3 fragNormal;
out vec3 fragPosition;
out vec3 fragA;
out vec3 fragD;
out vec4 fragS;

uniform float particleSize;
uniform vec3 cameraPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

vec3 normal()
{
	return normalize(cameraPos - gl_in[0].gl_Position.xyz).xyz;
}

void main()
{
	vec3 n = normal();
	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = cross(n, up);

	float size = particleSize/2;

	vec4 u4 = vec4(up*size, 0.0);
	//vec4 r4 = vec4(right*size, 0.0);

	gl_Position = projection*view*model*(gl_in[0].gl_Position + u4 - r4);
	EmitVertex();
	gl_Position = projection*view*model*(gl_in[0].gl_Position + u4 + r4);
	EmitVertex();
	gl_Position = projection*view*model*(gl_in[0].gl_Position - u4 + r4);
	EmitVertex();
	EndPrimitive();
	gl_Position = projection*view*model*(gl_in[0].gl_Position + u4 - r4);
	EmitVertex();
	gl_Position = projection*view*model*(gl_in[0].gl_Position - u4 - r4);
	EmitVertex();
	gl_Position = projection*view*model*(gl_in[0].gl_Position - u4 + r4);
	EmitVertex();
	EndPrimitive();
	//fragUV = UV[i];
	//fragA = a[i];
	//fragD = diff[i];
	//fragS = s[i];
	fragNormal = (model * vec4(normal(), 1.0f)).xyz;
	fragPosition = (model * gl_in[0].gl_Position).xyz;
}