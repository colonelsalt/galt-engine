#version 420 core

out vec4 FragColour;

const float AMBIENT_FACTOR = 0.2;

struct PointLight
{
	vec3 Position;
	vec3 Colour;
};
uniform PointLight u_PointLight;

uniform sampler2D u_DiffuseTexture;
uniform sampler2D u_SpecTexture;
uniform sampler2D u_NormalTexture;

uniform vec3 u_CameraPos;

in mat3 v_Tbn;
in vec3 v_FragPos;
in vec2 v_TexCoords;

void main()
{
	vec3 textureColour = texture(u_DiffuseTexture, v_TexCoords).xyz;

	vec3 viewDir = normalize(u_CameraPos - v_FragPos);
	
	vec4 normalMapValue = texture(u_NormalTexture, v_TexCoords);

	vec3 tangentSpaceNormal = normalize(normalMapValue.rgb * 2.0 - vec3(1.0));

	vec3 normal = normalize(v_Tbn * tangentSpaceNormal);

	vec3 ambient = textureColour * AMBIENT_FACTOR;
	vec3 lightDir = normalize(u_PointLight.Position - v_FragPos);
	float diffuseFactor = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diffuseFactor * textureColour * u_PointLight.Colour;

	vec3 reflectionDir = reflect(-lightDir, normal);
	float specIntensity = pow(max(dot(viewDir, reflectionDir), 0.0), 32.0);
	vec3 specular = texture(u_SpecTexture, v_TexCoords).xyz * specIntensity;

	FragColour = vec4(ambient + diffuse + specular, 1.0);
}