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
};

cbuffer LightBuffer : register(b0)
{
	float4 ambient;
	float4 diffuse;
	float4 direction;
	float4 position;

};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting (float3 lightDirection, float3 normal, float4 diffuse)
{
	float intensity = saturate(dot(normal, lightDirection));
	float4 colour = saturate(diffuse * intensity);
	return colour;
}

float4 main(InputType input) : SV_TARGET
{
	float4 lightColour;
	float4 textureColour = texture0.Sample(sampler0, input.tex);
	lightColour = calculateLighting(-direction, input.normal, diffuse);
	return lightColour;// *textureColour;
}