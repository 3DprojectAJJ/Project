#version 430

in vec2 fragmentUV;
in vec3 fragmentNormal;
in vec3 fragmentPosition;

layout(location = 0) out vec3 color;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 position;
layout(location = 3) out vec3 depth;

float deptha;
uniform sampler2D tex;

float LinearizeDepth(float zoverw){
		float n = 1.0; // camera z near
		float f = 20000.0; // camera z far
		return (2.0 * n) / (f + n - zoverw * (f - n));
	}

void main()
{
  color = texture(tex, fragmentUV).rgb;
  normal = fragmentNormal;
  position = fragmentPosition;
  deptha = LinearizeDepth(gl_FragCoord.z)*77;
  depth = vec3(deptha, deptha, deptha);
}