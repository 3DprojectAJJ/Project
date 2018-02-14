#version 430

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec2 geometryUV[];
in vec3 geometryNormal[];

out vec3 fragmentPosition;
out vec2 fragmentUV;
out vec3 fragmentNormal;

uniform mat4 View;
uniform mat4 Projection;
uniform mat4 Model;
uniform vec3 CameraPos;

vec3 normal()
{

	vec3 v1 = (gl_in[1].gl_Position - gl_in[0].gl_Position).xyz;
	vec3 v2 = (gl_in[2].gl_Position - gl_in[0].gl_Position).xyz;

	return normalize(cross(v1,v2));
}


void main()
{
	//mat4 Model = mat4(1.0f);

	vec3 averageNormal = (geometryNormal[0] + geometryNormal[1] + geometryNormal[2])/3;

	vec3 transformedNormal = normalize((Model * vec4(averageNormal, 1.0f)).xyz);

	vec3 middleOfTriangle = (gl_in[0].gl_Position.xyz + gl_in[1].gl_Position.xyz + gl_in[2].gl_Position.xyz)/3;

	vec3 viewVector = normalize((Model*vec4(middleOfTriangle,1.0f)).xyz - CameraPos);

	float d = dot(viewVector, transformedNormal);

	//if(d<0)
	//{
		for(int i = 0; i < 3; i++)
		{
			gl_Position = Projection * View * Model * gl_in[i].gl_Position;
			fragmentNormal = (Model * vec4(geometryNormal[i], 1.0f)).xyz;
			fragmentUV = geometryUV[i];
			fragmentPosition = (Model * gl_in[i].gl_Position).xyz;
			EmitVertex();
		}
		EndPrimitive();
	//}
}