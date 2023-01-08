struct InputType
{
	float4 position : SV_POSITION;
	float4 depthPosition : TEXCOORD1;
};

float4 main(InputType input) : SV_TARGET
{
	float depthValue;
	//get the depth value of the pixel by dividing the Z pixel depth by the homogeneous W coordinate.
	depthValue = input.depthPosition.z / input.depthPosition.w;
	return float4(depthValue, depthValue, depthValue, 1.0f);
}