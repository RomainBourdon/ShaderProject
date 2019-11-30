// Tessellation Hull Shader
// Prepares control points for tessellation
cbuffer factorBuffer: register(b0)
{
	float3 Camera;
	float time;
};

struct InputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 colour : COLOR;
	float3 worldPosition : TEXCOORD1;
};

struct ConstantOutputType
{
	float edges[4] : SV_TessFactor;
	float inside[2] : SV_InsideTessFactor;
};

struct OutputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 colour : COLOR;
	float3 worldPosition : TEXCOORD1;
};

ConstantOutputType PatchConstantFunction(InputPatch<InputType, 4> inputPatch, uint patchId : SV_PrimitiveID)
{
	ConstantOutputType output;
	float MaxTess = 64;

	float3 averageMidpoint;
	float tessFact;

	averageMidpoint = float3(inputPatch[0].position + inputPatch[1].position + inputPatch[2].position + inputPatch[3].position) / 4;

	float3 cameradist = float3(Camera.x - averageMidpoint.x, Camera.y - averageMidpoint.y, Camera.z - averageMidpoint.z);

	tessFact = MaxTess - (length(cameradist) * 2);

	if (tessFact < 1)
	{
		tessFact = 1;
	}

	output.edges[0] = tessFact;
	output.edges[1] = tessFact;
	output.edges[2] = tessFact;
	output.edges[3] = tessFact;

	output.inside[0] = tessFact;
	output.inside[1] = tessFact;

	return output;
}


[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchConstantFunction")]
OutputType main(InputPatch<InputType, 4> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
	OutputType output;


	// Set the position for this control point as the output position.
	output.position = patch[pointId].position;

	// Set the tex coord for this control point as the output tex coords.
	output.tex = patch[pointId].tex;

	// Set the input normals as the output normals.
	output.normal = patch[pointId].normal;

	// Set the input colour as the output colour.
	output.colour = patch[pointId].colour;

	output.worldPosition = patch[pointId].worldPosition;

	return output;
}