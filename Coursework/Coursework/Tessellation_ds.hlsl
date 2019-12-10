// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
};

cbuffer cameraBuffer: register(b1)
{
	float3 Camera;
	float heightmapmul;
};

struct ConstantOutputType
{
	float edges[4] : SV_TessFactor;
	float inside[2] : SV_InsideTessFactor;
};

struct InputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 worldPosition : TEXCOORD1;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 worldPosition : TEXCOORD1;
	float4 lightViewPos : TEXCOORD2;
	float4 depthPosition: TEXCOORD3;
};

float SampleHeightMap(float2 uv)
{
	//sample the height map from the offset tex coords
	const float SCALE = 3.0f;
	return SCALE * texture0.SampleLevel(sampler0, uv, 0.0f).r;
}

float3 Sobel(float2 tc)
{
	//get the height maps dimensions
	float HeightMapDimensionsX, HeightMapDimensionsY;
	texture0.GetDimensions(HeightMapDimensionsX, HeightMapDimensionsY);
	float2 pxSz = float2(1.0f / HeightMapDimensionsX, 1.0f / HeightMapDimensionsY);

	//set the offsets of the pixels
	float2 ooo = tc + float2(-pxSz.x, -pxSz.y);
	float2 o1o = tc + float2(0.0f, -pxSz.y);
	float2 o2o = tc + float2(pxSz.x, -pxSz.y);

	float2 o01 = tc + float2(-pxSz.x, 0.0f);
	float2 o21 = tc + float2(pxSz.x, 0.0f);

	float2 o02 = tc + float2(-pxSz.x, pxSz.y);
	float2 o12 = tc + float2(0.0f, pxSz.y);
	float2 o22 = tc + float2(pxSz.x, pxSz.y);


	float h00 = SampleHeightMap(ooo);
	float h10 = SampleHeightMap(o1o);
	float h20 = SampleHeightMap(o2o);

	float h01 = SampleHeightMap(o01);
	float h21 = SampleHeightMap(o21);

	float h02 = SampleHeightMap(o02);
	float h12 = SampleHeightMap(o12);
	float h22 = SampleHeightMap(o22);

	//combine th sampled height map coords
	float Gx = h00 - h20 + 2.0f * h01 - 2.0f * h21 + h02 - h22;
	float Gy = h00 + 2.0f * h10 + h20 - h02 - 2.0f * h12 - h22;

	//generate the z component
	float Gz = 0.5f * sqrt(max(0.1f, 1.0f - Gx * Gx - Gy * Gy));

	return normalize(float3(2.0f * Gx, Gz, 2.0f * Gy));
}

[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
	float3 vertexPosition;
	float2 UV;
	OutputType output;

	// Determine the position of the new vertex.
	// Invert the y and Z components of uvwCoord as these coords are generated in UV space and therefore y is positive downward.
	// Alternatively you can set the output topology of the hull shader to cw instead of ccw (or vice versa).

	float3 v1 = lerp(patch[0].position, patch[1].position, uvwCoord.y);
	float3 v2 = lerp(patch[3].position, patch[2].position, uvwCoord.y);
	vertexPosition = lerp(v1, v2, uvwCoord.x);

	float2 uv1 = lerp(patch[0].tex, patch[1].tex, uvwCoord.y);
	float2 uv2 = lerp(patch[3].tex, patch[2].tex, uvwCoord.y);
	UV = lerp(uv1, uv2, uvwCoord.x);

	float3 wp1 = lerp(patch[0].worldPosition, patch[1].worldPosition, uvwCoord.y);
	float3 wp2 = lerp(patch[3].worldPosition, patch[2].worldPosition, uvwCoord.y);
	output.worldPosition = lerp(wp1, wp2, uvwCoord.x);

	output.normal = Sobel(UV);
	output.normal = mul(output.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);

	float h = texture0.SampleLevel(sampler0, UV, 0).r;
	vertexPosition.y += (heightmapmul * h);// *float3(Normal);

	// Calculate the position of the new vertex against the world, view, and projection matrices.
	output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Calculate the position of the vertice as viewed by the light source.
	output.lightViewPos = mul(float4(vertexPosition, 1.0f), worldMatrix);
	output.lightViewPos = mul(output.lightViewPos, lightViewMatrix);
	output.lightViewPos = mul(output.lightViewPos, lightProjectionMatrix);

	output.tex = UV;

	output.depthPosition = output.position;

	return output;
}


