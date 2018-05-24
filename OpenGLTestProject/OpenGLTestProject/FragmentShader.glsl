#version 430

in vec2 fragUV;
in vec3 fragNormal;
in vec3 fragPosition;
in vec3 fragA;
in vec3 fragD;
in vec4 fragS;
in mat3 TBN;

uniform sampler2D tex;
uniform sampler2D normalMap;
uniform int useNormalMap;

layout(location = 0) out vec3 color;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec3 position;

layout(location = 3) out vec3 depth;
layout(location = 4) out vec3 ambient;
layout(location = 5) out vec3 diffuse;
layout(location = 6) out vec4 specular;

float LinearizeDepth(float zoverw){
		float n = 1.0; // camera z near
		float f = 20000.0; // camera z far
		return (2.0 * n) / (f + n - zoverw * (f - n));
	}

void main()
{
	color = texture(tex, fragUV).rgb;
	position = fragPosition;
	
	if(useNormalMap != 0){
		normal = TBN * (texture(normalMap, fragUV).rgb * 2.0 - vec3(1, 1, 1));
	}else{
		normal = fragNormal;
	}

	float deptha;
	deptha = LinearizeDepth(gl_FragCoord.z)*77;
	depth = vec3(deptha, deptha, deptha);
	ambient = fragA;
	diffuse = fragD;
	specular = fragS;
}