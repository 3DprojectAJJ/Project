#version 430


in vec2 UV;
in vec4 lightSpaceVector;

uniform mat4 lightSpaceMat;
uniform vec3 lightPosition[16];
uniform vec4 lightColor[16];
uniform vec3 camPos;

out vec3 color;

uniform int nrOfLights;

uniform sampler2D colorTexture;
uniform sampler2D normalTexture;
uniform sampler2D positionTexture;

uniform sampler2D ambientTexture;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

uniform sampler2D depthMap;

float ShadowCalculation(vec4 fragPosLightSpace)
{
	float shadow = 0.0f;

	//Do a perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	//Bring the projCoord in the 0 - 1 range
	projCoords = projCoords * 0.5 + 0.5;
	//Get the closest depth from the light perspective
	float closestDepth = texture(depthMap, projCoords.xy).r; 
	//Get depth of current fragment from the light perspective
	float currentDepth = projCoords.z;
	//Using a bias to pervent shadow acne
	float bias = 0.0005;
	// check whether current fragments position is in shadow
	if(currentDepth - bias > closestDepth)
		shadow = 1.0f;
	else
		shadow = 0.0f;

	return shadow;
}

void main(){
	vec3 normal = texture(normalTexture, UV).xyz;
	vec3 position = texture(positionTexture, UV).xyz;

	vec3 Ka = texture(ambientTexture, UV).xyz;
	vec3 Kd = texture(diffuseTexture, UV).xyz;
	vec3 Ks = texture(specularTexture, UV).xyz;
	float Ns = texture(specularTexture, UV).w;
	vec3 base = texture(colorTexture, UV).xyz;
	float shadow;  

	color = vec3(0.0f);

	if(normal.x == 0 && normal.y == 0 && normal.z == 0)
	{
		color = texture(colorTexture, UV).xyz;
	}
	else
	{
	// ambient
		vec3 final = Ka*base;

		shadow = ShadowCalculation(lightSpaceMat * vec4(position, 1.0f));

		vec3 eye_direction = normalize(camPos - position);
		for(int i = 0; i < nrOfLights; i++)
		{
			// diffuse
			vec3 light_ray = normalize(lightPosition[i] - position);
			float distance = length(lightPosition[i] - position);
			float cosTheta = dot(normal, light_ray);

			if(cosTheta > 0)
			{
				final += base * Kd * cosTheta * lightColor[i].xyz * (lightColor[i].w)* (1/(distance * distance)) * (1-shadow); 
			}

			// specular
			vec3 reflect_ray = reflect(-light_ray, normal);
			float cosAlpha = clamp(dot(eye_direction, reflect_ray), 0, 1);

			if(cosTheta > 0)
			{
				final += Ks * pow(cosAlpha, Ns) * lightColor[i].xyz * (lightColor[i].w)* (1/(distance * distance)) * (1-shadow);
			}
		}
		color = final;
	}
}