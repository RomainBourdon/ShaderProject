cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer factorBuffer: register(b1)
{
	float3 Camera;
	float time;
};

struct InputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct OutputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 colour : COLOR;
	float3 worldPosition : TEXCOORD1;
	float4 lightViewPos : TEXCOORD2;
};

OutputType main(InputType input)
{
	OutputType output;

	// Pass the vertex position into the hull shader.
	output.position = input.position;

	output.tex = input.tex;

	output.normal = input.normal;

	// Pass the input color into the hull shader.
	output.colour = float4(1.0, 0.0, 0.0, 1.0);

	float4 worldPosition = mul(input.position, worldMatrix);
	output.worldPosition = worldPosition;

	output.lightViewPos = float4(0.0, 0.0, 0.0, 1.0);

	return output;
}
