#version 420 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoords;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_Bitangent;

layout (std140, binding = 0) uniform ProjView
{
	mat4 u_Projection;
	mat4 u_View;
};
uniform mat4 u_Model;

out vec3 v_FragPos;
out vec2 v_TexCoords;
out mat3 v_Tbn;

void main()
{
	vec4 worldSpacePos = u_Model * vec4(a_Position, 1.0);
	v_FragPos = worldSpacePos.xyz;
	v_TexCoords = a_TexCoords;

	vec3 tangent = normalize(vec3(u_Model * vec4(a_Tangent, 0.0)));
	vec3 normal = normalize(vec3(u_Model * vec4(a_Normal, 0.0)));
	vec3 bitangent = normalize(vec3(u_Model * vec4(a_Bitangent, 0.0)));
	v_Tbn = mat3(tangent, bitangent, normal);

	gl_Position = u_Projection * u_View * worldSpacePos;
}