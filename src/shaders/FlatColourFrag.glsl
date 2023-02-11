#version 420 core

out vec4 Colour;

uniform vec3 u_Colour;

void main()
{
	Colour = vec4(u_Colour, 1.0);
}