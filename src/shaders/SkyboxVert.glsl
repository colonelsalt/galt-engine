#version 420 core

layout(location = 0) in vec3 a_Position;

out vec3 v_TexCoords;

layout(std140, binding = 0) uniform ProjView
{
	mat4 u_Projection;
	mat4 u_View;
};

uniform mat4 u_TrimmedView;

void main()
{
	// Local interpolated vertex position gives 3D texture coordinate of cubemap
	v_TexCoords = a_Position;

	vec4 realPos = u_Projection * u_TrimmedView * vec4(a_Position, 1.0);

	// Set z component of position to w, so it ends up as 1.0 in the fragment shader (max. possible depth)
	gl_Position = realPos.xyww;
}