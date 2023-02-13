#version 420 core

in vec3 v_TexCoords;

out vec4 colour;

uniform samplerCube u_SkyboxTexture;

void main()
{
	colour = texture(u_SkyboxTexture, v_TexCoords);
}