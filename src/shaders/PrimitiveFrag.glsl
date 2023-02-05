#version 420 core

out vec4 colour;

uniform vec3 u_Colour;

void main()
{
	colour = vec4(u_Colour, 1.0);
}