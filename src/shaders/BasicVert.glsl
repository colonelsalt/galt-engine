#version 420 core

layout (location = 0) in vec3 a_Position;

uniform float u_OffsetX;
uniform float u_OffsetY;

void main()
{
	float x = a_Position.x + u_OffsetX;
	float y = a_Position.y + u_OffsetY;
	gl_Position = vec4(x, y, a_Position.z, 1.0);
}