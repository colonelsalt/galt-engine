#version 420 core

layout (location = 0) in vec3 a_Position;

uniform vec2 u_Offset;

void main()
{
	float x = a_Position.x + u_Offset.x;
	float y = a_Position.y + u_Offset.y;
	gl_Position = vec4(x, y, a_Position.z, 1.0);
}