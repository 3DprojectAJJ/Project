#version 430

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 geometryColor[];

out vec3 fragmentColor;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

void main()
{
	for(int i = 0; i < 3; i++)
	{
		gl_Position = Projection * View * Model * gl_in[i].gl_Position;
		fragmentColor = geometryColor[i];
		EmitVertex();
	}
	EndPrimitive();
}