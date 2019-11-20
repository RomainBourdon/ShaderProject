// Texture pixel/fragment shader
// Basic fragment shader for rendering textured geometry

// Texture and sampler registers
Texture2D textureScene : register(t0);
Texture2D textureBlur : register(t1);
Texture2D depthmap : register(t2);
SamplerState Sampler0 : register(s0);
SamplerState SamplerDepth : register(s1);

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

cbuffer CameraBuffer : register(b0)
{
	float3 position;
	float padding;
}

float4 main(InputType input) : SV_TARGET
{
	float4 FinaltextureColor;
	float4 textureNonblur = textureScene.Sample(Sampler0, input.tex);
	float4 textureblur = textureBlur.Sample(Sampler0, input.tex);
	float depthValue;
	float cameraDepthValue;

	float2 pTexCoord = input.position.xy / input.position.w;
	pTexCoord *= float2(0.5, -0.5);
	pTexCoord += float2(0.5f, 0.5f);

	depthValue = depthmap.Sample(SamplerDepth, pTexCoord).r;

	cameraDepthValue = position.z / 1.0f;
	cameraDepthValue -= 0.005f;

	if (pTexCoord.x < 0.f || pTexCoord.x > 1.f || pTexCoord.y < 0.f || pTexCoord.y > 1.f)
	{
		return textureblur;
	}

	if (cameraDepthValue < depthValue)
	{
		// Sample the pixel color from the texture using the sampler at this texture coordinate location.
		FinaltextureColor = textureNonblur;// +textureblur;
	}
	return FinaltextureColor;
}