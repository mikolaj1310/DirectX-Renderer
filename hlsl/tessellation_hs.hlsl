cbuffer testBuffer : register(b0)
{
	float3 camPos;
	float pad;
};

struct InputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float4 sv : SV_Position;
};

struct ConstantOutputType
{
	float edges[4] : SV_TessFactor;
	float inside[2] : SV_InsideTessFactor;
};

struct OutputType
{
	float2 tex : TEXCOORD4;
	float3 position : POSITION;
};

ConstantOutputType PatchConstantFunction(InputPatch<InputType, 4> inputPatch, uint patchId : SV_PrimitiveID)
{
	ConstantOutputType output;

	//get average input
	float3 inputAvg = (float3)0;
	for (int i = 0; i < 4; i++)
	{
		inputAvg += inputPatch[i].position;
	}
	inputAvg = inputAvg / 4;
	//convert float 3 to float 2
	float2 pos = float2(inputAvg.x, inputAvg.z);

	//get distance from vertex to camera
	float2 distanceToCam = (pos - camPos.xz);

	//find distance average
	float distAvg = (distanceToCam.x + distanceToCam.y) / 2;
	distAvg += 1 / 2;

	//flip distance to positive
	if (distAvg < 0)
		distAvg *= -1;

	//Calculate tesseleation and clamp between 1 and 4
	float tessellationFactor = clamp(100 / distAvg, 1, 4);

	output.inside[0] = tessellationFactor;
	output.inside[1] = tessellationFactor;

	//set tesselation factor for each edge
	output.edges[0] = tessellationFactor;
	output.edges[1] = tessellationFactor;
	output.edges[2] = tessellationFactor;
	output.edges[3] = tessellationFactor;


	return output;
}


[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchConstantFunction")]
OutputType main(InputPatch<InputType, 4> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
	OutputType output;

	output.position = patch[pointId].position;
	output.tex = patch[pointId].tex;

	return output;
}