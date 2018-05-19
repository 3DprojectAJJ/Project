#version 430

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in vec2 UV[];
in vec3 a[];
in vec3 diff[];
in vec4 s[];
in vec3 mTangents[];
in vec3 mBitangents[];

out vec2 fragUV;
out vec3 fragNormal;
out vec3 fragPosition;
out vec3 fragA;
out vec3 fragD;
out vec4 fragS;

out mat3 TBN;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

vec3 calcNormal()
{

	vec3 v1 = (gl_in[1].gl_Position - gl_in[0].gl_Position).xyz;
	vec3 v2 = (gl_in[2].gl_Position - gl_in[0].gl_Position).xyz;
	return normalize(cross(v1,v2));
}

void main()
{
	vec3 normal = calcNormal();
	vec3 viewVector = (view*model*(gl_in[0].gl_Position + gl_in[1].gl_Position + gl_in[2].gl_Position)).xyz/3;
	float d = dot(viewVector,(view * model * vec4(normal, 0)).xyz);

	if(d<0)
	{

	vec3 mNormal = normalize(model * vec4(normal, 0.0f)).xyz;

	TBN = mat3(mTangents[0], mBitangents[0], mNormal);

	fragNormal = mNormal;

		for(int i = 0; i < 3; i++)
		{
			fragUV = UV[i];
			fragA = a[i];
			fragD = diff[i];
			fragS = s[i];
			
			fragPosition = (model * gl_in[i].gl_Position).xyz;
			gl_Position = projection*view*model*gl_in[i].gl_Position;
			EmitVertex();
		}
		EndPrimitive();
	}
}