#version 430


in vec2 UV;

uniform vec3 lightPosition[16];
uniform vec4 lightColor[16];
uniform vec3 camPos;

out vec3 color;

uniform int nrOfLights;

uniform sampler2D colorTexture;
uniform sampler2D normalTexture;
uniform sampler2D positionTexture;
//uniform sampler2D depthTexture;

uniform sampler2D ambientTexture;
uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

void main(){
	vec3 normal = texture(normalTexture, UV).xyz;
	vec3 position = texture(positionTexture, UV).xyz;

	vec3 Ka = texture(ambientTexture, UV).xyz;
	vec3 Kd = texture(diffuseTexture, UV).xyz;
	vec3 Ks = texture(specularTexture, UV).xyz;
	float Ns = texture(specularTexture, UV).w;
	vec3 base = texture(colorTexture, UV).xyz;


	color = vec3(0.0f);

	if(normal.x == 0 && normal.y == 0 && normal.z == 0)
	{
		color = texture(colorTexture, UV).xyz;
	}
	else
	{
	// ambient
		vec3 final = Ka*base;


		vec3 eye_direction = normalize(camPos - position);
		for(int i = 0; i < nrOfLights; i++)
		{
			/*float diffuse = dot(normal, normalize(lightPosition[i] - position));
			float distance = length(lightPosition[i] - position);
			color += clamp(lightColor[i].xyz * diffuse * texture(colorTexture, UV).xyz * 1/(distance * distance) * lightColor[i].w, 0, 1);*/

			// diffuse
			vec3 light_ray = normalize(lightPosition[i] - position);
			float distance = length(lightPosition[i] - position);
			float cosTheta = dot(normal, light_ray);

			if(cosTheta > 0)
			{
				final += base * Kd * cosTheta * lightColor[i].xyz * (lightColor[i].x + lightColor[i].y + lightColor[i].z); 
			}

			// specular
			vec3 reflect_ray = reflect(-light_ray,normal);
			float cosAlpha = clamp(dot(eye_direction, reflect_ray), 0, 1);
			if(cosTheta > 0)
			{
				final += Ks * pow(cosAlpha,Ns)*lightColor[i].xyz * (lightColor[i].x + lightColor[i].y + lightColor[i].z); 
			}
		}
		color = final;
	}
}