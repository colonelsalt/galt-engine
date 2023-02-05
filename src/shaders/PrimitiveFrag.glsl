#version 420 core

out vec4 colour;

uniform sampler2D u_Texture;

in vec2 v_TexCoords;

void main()
{
	colour = texture(u_Texture, v_TexCoords);
}