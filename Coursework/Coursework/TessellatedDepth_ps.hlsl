struct InputType
{
float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 colour : COLOR;
	float3 worldPosition : TEXCOORD1;
	float4 lightViewPos : TEXCOORD2;
	float4 depthPosition: TEXCOORD3;
};

float4 main(InputType input) : SV_TARGET
{
	float depthValue;
	// Get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate.
	depthValue = input.depthPosition.z / input.depthPosition.w;

	return float4(depthValue, depthValue, depthValue, 1.0f);
}