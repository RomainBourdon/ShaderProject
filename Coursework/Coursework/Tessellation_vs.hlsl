// Tessellation vertex shader.
// vertex shader Doesn't do much, could manipulate the control points
// Pass forward data, strip out some values not required for example.
cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
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
	float3 worldPosition : TEXCOORD1;
};

OutputType main(InputType input)
{
	OutputType output;

	// Pass the vertex position into the hull shader.
	output.position = input.position;

	//pass texcoords into hull shader
	output.tex = input.tex;

	//pass normals into hull shader
	output.normal = input.normal;

	//calculate and pass the world position for the hull shader
	float4 worldPosition = mul(input.position, worldMatrix);
	output.worldPosition = worldPosition;

	return output;
}
