#version 430


in vec2 UV;

uniform vec3 lightPosition[16];
uniform vec4 lightColor[16];

out vec3 color;

uniform int nrOfLights;

uniform sampler2D colorTexture;
uniform sampler2D normalTexture;
uniform sampler2D positionTexture;
uniform sampler2D depthTexture;
uniform sampler2D ambientTexture;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

void main(){
	vec3 normal = texture(normalTexture, UV).xyz;
	vec3 position = texture(positionTexture, UV).xyz;

	color = vec3(0.0f);

	if(normal.x == 0 && normal.y == 0 && normal.z == 0){
		color = texture(colorTexture, UV).xyz;
	}else{
		for(int i = 0; i < nrOfLights; i++){
		float diffuse = dot(normal, normalize(lightPosition[i] - position));
		float distance = length(lightPosition[i] - position);

		color += clamp(lightColor[i].xyz * diffuse * texture(colorTexture, UV).xyz * 1/(distance * distance) * lightColor[i].w, 0, 1);
		}
	}
}