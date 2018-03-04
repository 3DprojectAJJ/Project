#version 430

in vec2 UV;

out vec3 color;

uniform sampler2D colorTexture;
uniform sampler2D normalTexture;
uniform sampler2D positionTexture;
uniform sampler2D depthTexture;
uniform sampler2D ambientTexture;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

void main(){
	vec3 normal = normalize(texture(normalTexture, UV).xyz);
	vec3 position = texture(positionTexture, UV).xyz;

	vec4 light = vec4(4, 3, 3, 10);

	float diffuse = dot(normal, normalize(light.xyz - position));
	float distance = length(light.xyz - position);

    color = diffuse * texture(colorTexture, UV).xyz * 1/(distance * distance) * light.w;
}