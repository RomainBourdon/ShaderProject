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
	float tessFact[5];
	float3 cameradist[5];
	float3 averageLineMidPoint[5];

	averageLineMidPoint[0] = lerp(inputPatch[0].position, inputPatch[1].position, 0.5f);
	averageLineMidPoint[1] = lerp(inputPatch[1].position, inputPatch[2].position, 0.5f); 
	averageLineMidPoint[2] = lerp(inputPatch[2].position, inputPatch[3].position, 0.5f);
	averageLineMidPoint[3] = lerp(inputPatch[3].position, inputPatch[0].position, 0.5f);
	averageLineMidPoint[4] = float3(inputPatch[0].position + inputPatch[1].position + inputPatch[2].position + inputPatch[3].position) / 4;

	for (int i = 0; i < 5; ++i)
	{
		cameradist[i] = float3(Camera.x - averageLineMidPoint[i].x, Camera.y - averageLineMidPoint[i].y, Camera.z - averageLineMidPoint[i].z);
		tessFact[i] = MaxTess - (length(cameradist[i]) * 2);
		if (tessFact[i] < 1)
		{
			tessFact[i] = 1;
		}
	}	

	output.edges[0] = tessFact[0];
	output.edges[1] = tessFact[3];
	output.edges[2] = tessFact[2];
	output.edges[3] = tessFact[1];

	output.inside[0] = tessFact[4];
	output.inside[1] = tessFact[4];

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