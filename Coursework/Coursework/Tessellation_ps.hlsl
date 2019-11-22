// Tessellation pixel shader
// Output colour passed to stage.

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);
Texture2D ShadowTexture : register(t1);
SamplerState shadowSampler : register(s1);

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 colour : COLOR;
	float3 worldPosition : TEXCOORD1;
	float4 lightViewPos : TEXCOORD2;
};

cbuffer LightBuffer : register(b0)
{
	float4 ambient[4];
	float4 diffuse[4];
	float4 direction[4];
	float4 position[3];

};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting (float3 lightDirection, float3 normal, float4 diffuse)
{
	float intensity = saturate(dot(normal, lightDirection));
	float4 colour = saturate(diffuse * intensity);
	return colour;
}

float4 calculatePointLighting(float3 lightDirection, float3 normal, float4 ldiffuse, float att) 
{
	float intensity = saturate(dot(normal, lightDirection));
	float4 colour = saturate(ldiffuse * intensity) * att;
	return colour;
}

float4 main(InputType input) : SV_TARGET
{
	float4 lightColour[4];
	float consFactor = 1.0f, linear_factor = 0.125f, quadratic = 0.0f;
	float3 lightVector[3];
	float distance[3];
	float attenuation[3];
	float cosa[3];
	float denomenator[3];

	float depthValue;
	float lightDepthValue;
	float shadowMapBias = 0.005f;
	//float4 colour = float4(0.f, 0.f, 0.f, 1.f);

	float4 textureColour = texture0.Sample(sampler0, input.tex);

	float2 pTexCoord = input.lightViewPos.xy / input.lightViewPos.w;
	pTexCoord *= float2(0.5, -0.5);
	pTexCoord += float2(0.5f, 0.5f);

	if (pTexCoord.x < 0.f || pTexCoord.x > 1.f || pTexCoord.y < 0.f || pTexCoord.y > 1.f)
	{
		return textureColour;
	}

	// Sample the shadow map (get depth of geometry)
	depthValue = ShadowTexture.Sample(shadowSampler, pTexCoord).r;
	// Calculate the depth from the light.
	lightDepthValue = input.lightViewPos.z / input.lightViewPos.w;
	lightDepthValue -= shadowMapBias;

	// Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
	if (lightDepthValue < depthValue)
	{
		//colour = calculateLighting(-direction, input.normal, diffuse);

		lightColour[0] = calculateLighting(-direction[0], input.normal, diffuse[0]);
	}

	for (int i = 0; i < 3; i++)
	{
		lightVector[i] = position[i] - input.worldPosition;
		distance[i] = length(lightVector[i]);
		lightVector[i] = normalize(lightVector[i]);
		cosa[i] = dot(direction[i + 1], -lightVector[i]);
		denomenator[i] = (consFactor + (linear_factor * distance[i]) + (quadratic * pow(distance[i], 2)));

		if (cosa[i] > cos(3.14159f / 10.0f))
		{
			attenuation[i] = 1 / denomenator[i];
			if (denomenator[i] > 1)
			{
				attenuation[i] = 1;
			}
			lightColour[i + 1] = calculatePointLighting(lightVector[i], input.normal, diffuse[i + 1], attenuation[i]) + ambient[i + 1];
		}
		else
		{
			lightColour[i + 1] = ambient[i + 1];
		}
	}

	lightColour[0] = saturate(lightColour[0] + ambient[0]);
	
	return (saturate(lightColour[0]) + lightColour[1] + lightColour[2] + lightColour[3]) *textureColour;
}