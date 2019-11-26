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

cbuffer DepthBuffer : register(b0)
{
	float nearest;
	float farest;
	float offset;
	float range;
}

float4 main(InputType input) : SV_TARGET
{
	float4 FinaltextureColor;
	float4 textureNonblur = textureScene.Sample(Sampler0, input.tex);
	float4 textureblur = textureBlur.Sample(Sampler0, input.tex);

	float depthValue = depthmap.Sample(Sampler0, input.tex).r;

	float centreDepth = depthmap.Sample(Sampler0, float2(0.5f, 0.5f)).r;

	depthValue = 1 - depthValue;
	centreDepth = 1 - centreDepth;

	centreDepth *= (farest - nearest);
	depthValue *= (farest - nearest);

	float blurFac = saturate(abs(depthValue - centreDepth - offset) / range);

	return lerp(textureNonblur, textureblur, blurFac);
}