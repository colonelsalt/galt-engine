#version 420 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoords;
layout (location = 3) in vec3 a_Tangent;
layout (location = 4) in vec3 a_Bitangent;
layout (location = 5) in ivec4 a_BoneIds;
layout (location = 6) in vec4 a_BoneWeights;

layout(std140, binding = 0) uniform ProjView
{
	mat4 u_Projection;
	mat4 u_View;
};
uniform mat4 u_Model;

const int MAX_TOTAL_BONES = 100;
const int MAX_BONES_PER_VERTEX = 4;

// TODO: Convert to uniform buffer
layout(std140, binding = 1) uniform SkinBuffer
{
	uniform mat4 u_SkinningMatrices[MAX_TOTAL_BONES];
};

out vec2 v_TexCoords;
out mat3 v_Tbn;
out vec3 v_FragPos;

void main()
{
	vec4 finalPosition = vec4(0.0);
	vec3 normal = vec3(0.0);
	vec3 tangent = vec3(0.0);
	vec3 bitangent = vec3(0.0);

	// Go through all joints attached to this vertex, and sum up their contribution to the final pos/rot of the vertex
	for (int i = 0; i < MAX_BONES_PER_VERTEX; i++)
	{
		if (a_BoneIds[i] == -1)
		{
			continue;
		}
		if (a_BoneIds[i] >= MAX_TOTAL_BONES)
		{
			// This should never happen
			finalPosition = vec4(a_Position, 1.0);
			break;
		}
	
		mat4 skinningMatrix = u_SkinningMatrices[a_BoneIds[i]];
		vec4 bonePosition = skinningMatrix * vec4(a_Position, 1.0);
		finalPosition += bonePosition * a_BoneWeights[i];
		normal += mat3(skinningMatrix) * a_Normal;
		tangent += mat3(skinningMatrix) * a_Tangent;
		bitangent += mat3(skinningMatrix) * a_Bitangent;
	}

	vec4 worldSpacePos = u_Model * finalPosition;
	v_FragPos = worldSpacePos.xyz;
	gl_Position = u_Projection * u_View * worldSpacePos;
	v_TexCoords = a_TexCoords;

	vec3 T = normalize(vec3(u_Model * vec4(tangent, 0.0)));
	vec3 B = normalize(vec3(u_Model * vec4(bitangent, 0.0)));
	vec3 N = normalize(vec3(u_Model * vec4(normal, 0.0)));
	v_Tbn = mat3(T, B, N);
}