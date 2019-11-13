// Tessellation pixel shader
// Output colour passed to stage.

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 colour : COLOR;
	float3 worldPosition : TEXCOORD1;
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
	float intensity = saturate(dot(normal, lightDirection)) * att;
	float4 colour = saturate(ldiffuse * intensity);
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
	float4 textureColour = texture0.Sample(sampler0, input.tex);
	lightColour[0] = calculateLighting(-direction[0], input.normal, diffuse[0]);

	for (int i = 0; i < 3; i++)
	{
		lightVector[i] = position[i] - input.worldPosition;
		distance[i] = length(lightVector[i]);
		lightVector[i] = normalize(lightVector[i]);
		cosa[i] = dot(direction[i+1], -lightVector[i]);
		
		if (cosa[i] > cos(3.14159f / 10.0f))
		{	
			attenuation[i] = 1 / (consFactor + (linear_factor * distance[i]) + (quadratic * pow(distance[i], 2)));
			lightColour[i+1] = calculatePointLighting(lightVector[i], input.normal, diffuse[i+1], attenuation[i]) + ambient[i+1];
		}
		else
		{
			lightColour[i+1] = ambient[i+1];
		}
	}
	return (lightColour[0] + lightColour[1] + lightColour[2] + lightColour[3]) *textureColour;
}