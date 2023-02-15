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
uniform sampler2D u_ShadowMapTexture;
uniform vec3 u_CameraPos;
uniform int u_UsesSpec;

in vec3 v_Normal;
in vec3 v_FragPos;
in vec2 v_TexCoords;
in vec4 v_FragPosLightSpace;

float Shadow(float bias)
{
	vec3 ndc = (v_FragPosLightSpace.xyz / v_FragPosLightSpace.w) * 0.5 + 0.5;
	if (ndc.z > 1.0)
		return 0.0;
	vec2 texelSize = 1.0 / textureSize(u_ShadowMapTexture, 0);
	float shadow = 0.0;
	for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			vec2 neighbourPixel = ndc.xy + vec2(x, y) * texelSize;
			float sampleDepth = texture(u_ShadowMapTexture, neighbourPixel).r;
			if (ndc.z - bias > sampleDepth)
			{
				shadow += 1.0;
			}
		}
	}
	return shadow / 9.0;
}

void main()
{
	vec3 textureColour = texture(u_DiffuseTexture, v_TexCoords).xyz;

	vec3 viewDir = normalize(u_CameraPos - v_FragPos);
	vec3 normal = normalize(v_Normal);

	vec3 ambient = textureColour * AMBIENT_FACTOR;
	vec3 lightDir = normalize(u_PointLight.Position - v_FragPos);
	float diffuseFactor = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diffuseFactor * textureColour * u_PointLight.Colour;

	vec3 reflectionDir = reflect(-lightDir, normal);
	float specIntensity = pow(max(dot(viewDir, reflectionDir), 0.0), 32.0);
	
	vec3 specular = vec3(0.0);
	if (u_UsesSpec == 1)
	{
		specular = texture(u_SpecTexture, v_TexCoords).xyz * specIntensity;
	}

	float bias = 0.0;
	float shadowFactor = Shadow(bias);

	FragColour = vec4(ambient + (1.0 - shadowFactor) * (diffuse + specular), 1.0);
}