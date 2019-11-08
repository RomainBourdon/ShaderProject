// Light pixel shader
// Calculate diffuse lighting for a single directional light (also texturing)

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer LightBuffer : register(b0)
{
	float4 ambient[2];
	float4 diffuse[2];
	float4 direction[2];
	float4 specColour;
	float4 position[2];
	float4 power;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewVector : TEXCOORD1;
	float3 worldPosition : TEXCOORD2;
};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 ldiffuse) //, float att)
{
	float intensity = saturate(dot(normal, lightDirection));// * att;
	float4 colour = saturate(ldiffuse * intensity);
	return colour;
}

float4 calcSpecular(float3 lightDirection, float3 normal, float3 viewVector, float4 specularColour, float specularPower)
{
	float3 halfway = normalize(lightDirection + viewVector);
	float SpecularIntensity = pow(max(dot(normal, halfway), 0.0), specularPower);
	return saturate(specularColour * SpecularIntensity);

}

float4 main(InputType input) : SV_TARGET
{
	// Sample the texture. Calculate light intensity and colour, return light*texture for final pixel colour.
	float4 textureColour = texture0.Sample(sampler0, input.tex);
	float consFactor = 1.0f, linear_factor = 0.125f, quadratic = 0.0f;
	//float4 finalspec;
	float3 lightVector[2];
	//float distance[2];
	float4 lightColour[2];
	//float attenuation[2];

	for (int i = 0; i < 2; i++)
	{
		//finalspec = calcSpecular(direction[i], input.normal, input.worldPosition, specColour, power);
		lightVector[i] = normalize(position[i] - input.worldPosition);
		/*
		distance[i] = length(lightVector[i]);
		lightVector[i] = normalize(lightVector[i]);
		attenuation[i] = 1 / (consFactor + (linear_factor * distance[i]) + (quadratic * pow(distance[i], 2)));*/
		//lightColour[i] = calculateLighting(lightVector[i], input.normal, diffuse[i], attenuation[i]) + ambient[i];
		lightColour[i] = calculateLighting(lightVector[i], input.normal, diffuse[i]) + ambient[i];
	}


	return ((lightColour[0] * lightColour[1]) * textureColour);// + finalspec;
}



