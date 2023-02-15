#version 420 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;

layout (std140, binding = 0) uniform ProjView
{
	mat4 u_Projection;
	mat4 u_View;
};

uniform mat4 u_Model;
uniform mat4 u_LightProj;
uniform mat4 u_LightView;

out vec3 v_FragPos;
out vec3 v_Normal;
out vec2 v_TexCoords;
out vec4 v_FragPosLightSpace;

void main()
{
	vec4 worldSpacePos = u_Model * vec4(a_Position, 1.0);
	v_FragPos = worldSpacePos.xyz;
	v_FragPosLightSpace = u_LightProj * u_LightView * worldSpacePos;
	v_Normal = transpose(inverse(mat3(u_Model))) * a_Normal;
	v_TexCoords = a_TexCoords;
	gl_Position = u_Projection * u_View * worldSpacePos;
}