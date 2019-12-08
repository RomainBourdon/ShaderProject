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
	//sample the scene texture
	float4 FinaltextureColor;
	float4 textureNonblur = textureScene.Sample(Sampler0, input.tex);

	//sample the blur scene
	float4 textureblur = textureBlur.Sample(Sampler0, input.tex);

	//get the depth value of the scene
	float depthValue = depthmap.Sample(SamplerDepth, input.tex).r;

	//get the depth value of the centre of the texture
	float centreDepth = depthmap.Sample(SamplerDepth, float2(0.5f, 0.5f)).r;

	//make the depth values less then 1
	depthValue = 1 - depthValue;
	centreDepth = 1 - centreDepth;

	//get the distance of the depth value
	centreDepth *= (farest - nearest);
	depthValue *= (farest - nearest);

	//calculate the blur factor
	float blurFac = saturate(abs(depthValue - centreDepth - offset) / range);

	//interpolate between the scene textures
	return lerp(textureNonblur, textureblur, blurFac);
}