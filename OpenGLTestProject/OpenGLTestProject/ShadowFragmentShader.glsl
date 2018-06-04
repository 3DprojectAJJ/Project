#version 430
in vec4 FragPos;

void main()
{
	// Because we have no light calculations we leave this shader empty
	// because what we need for our shadowmapping basically happens behind the scene in the fragmentshader as shown below
	// gl_FragDepth = gl_FragPos.z;
}  