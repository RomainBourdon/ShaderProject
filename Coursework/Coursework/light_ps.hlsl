// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer LightBuffer : register(b0)
{
	float4 ambient[3];
	float4 diffuse[3];
	float4 position[3];

};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 worldPosition : TEXCOORD1;
	float4 colour : COLOR;
};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 ldiffuse, float att) //, float att)
{
	float intensity = saturate(dot(normal, lightDirection))* att;
	float4 colour = saturate(ldiffuse * intensity);
	return colour;
}

float4 main(InputType input) : SV_TARGET
{
	// Sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour.
	float4 textureColour = texture0.Sample(sampler0, input.tex);
	float consFactor = 1.0f, linear_factor = 0.125f, quadratic = 0.0f;
	float3 lightVector[3];
	float distance[3];
	float4 lightColour[3];
	float attenuation[3];

	for (int i = 0; i < 3; i++)
	{
		
		lightVector[i] = normalize(position[i] - input.worldPosition);
		
		distance[i] = length(lightVector[i]);
		lightVector[i] = normalize(lightVector[i]);
		attenuation[i] = 1 / (consFactor + (linear_factor * distance[i]) + (quadratic * pow(distance[i], 2)));
		lightColour[i] = calculateLighting(lightVector[i], input.normal, diffuse[i], attenuation[i]) + ambient[i];
		//lightColour[i] = calculateLighting(lightVector[i], input.normal, diffuse[i]) + ambient[i];
	}


	return input.colour;// ((lightColour[0] + lightColour[1] + lightColour[2]));// *textureColour);
}



