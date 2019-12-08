//pass in the texture and sampler
Texture2D shaderTexture: register(t0);
SamplerState SampleType: register(s0);

cbuffer ScreenSizeBuffer: register(b0)
{
	float screenWidth;
	float screenHeight;
	float2 padding;
};
struct InputType
{
	float4 position: SV_POSITION;
	float2 tex: TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
	float4 colour;
	// Initialize the colour to black.
	colour = float4(0.0f, 0.0f, 0.0f, 0.0f);
	// Determine the floating point size of a texelfor a screen with this specific width.
	float texelWidth = 1.0f / screenWidth;
	float texelHeight = 1.0f / screenHeight;

	// Add the nine pixels to the colour.
	colour += shaderTexture.Sample(SampleType, input.tex);
	colour += shaderTexture.Sample(SampleType, input.tex + float2(-texelWidth, -texelHeight));
	colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelHeight));
	colour += shaderTexture.Sample(SampleType, input.tex + float2(texelWidth, -texelHeight));
	colour += shaderTexture.Sample(SampleType, input.tex + float2(-texelWidth, 0.0f));
	colour += shaderTexture.Sample(SampleType, input.tex + float2(texelWidth, 0.0f));
	colour += shaderTexture.Sample(SampleType, input.tex + float2(-texelWidth, texelHeight));
	colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelHeight));
	colour += shaderTexture.Sample(SampleType, input.tex + float2(texelWidth, texelHeight));

	colour /= 9.0f;

	// Set the alpha channel to one.
	colour.a = 1.0f;
	return colour;
}