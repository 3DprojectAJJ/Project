#version 430

in vec2 fragUV;
in vec3 fragColor;
in vec4 fragPosition;

uniform vec3 forward;
uniform sampler2D tex;

layout(location = 0) out vec3 color;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 position;

layout(location = 3) out vec3 depth;
layout(location = 4) out vec3 diffuse;
layout(location = 5) out vec4 specular;
layout(location = 6) out vec3 ambient;

float LinearizeDepth(float zoverw){
		float n = 1.0; // camera z near
		float f = 20000.0; // camera z far
		return (2.0 * n) / (f + n - zoverw * (f - n));
	}

void main()
{
	color = clamp(fragColor, 0, 1);
	position = fragPosition.xyz;
	normal = vec3(0.0f, 0.0f, 0.0f);

	float deptha;
	deptha = LinearizeDepth(gl_FragCoord.z)*77;
	depth = vec3(deptha, deptha, deptha);
}