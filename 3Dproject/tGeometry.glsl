#version 430

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec2 geometryUV[];

out vec3 fragmentPosition;
out vec2 fragmentUV;
out vec3 fragmentNormal;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 CameraPos;

vec3 normal()
{

	vec3 v1 = (gl_in[1].gl_Position - gl_in[0].gl_Position).xyz;
	vec3 v2 = (gl_in[2].gl_Position - gl_in[0].gl_Position).xyz;

	return normalize(cross(v1,v2));
}


void main()
{		
		mat4 t = mat4(1.0f);

		for(int i = 0; i < 3; i++)
		{
			gl_Position = Projection * View * t * gl_in[i].gl_Position;
			fragmentNormal = (t * vec4(normal(), 1.0f)).xyz;
			fragmentUV = geometryUV[i];
			fragmentPosition = (t * gl_in[i].gl_Position).xyz;
			EmitVertex();
		}
		EndPrimitive();
}