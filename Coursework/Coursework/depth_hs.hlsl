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
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
};

ConstantOutputType PatchConstantFunction(InputPatch<InputType, 4> inputPatch, uint patchId : SV_PrimitiveID)
{
	ConstantOutputType output;

	float tessellate = 5;

	//tesselation factor for inside triangle
	output.inside[0] = tessellate;
	output.inside[1] = tessellate;

	//tesselation factor for edges of triangle
	output.edges[0] = tessellate;
	output.edges[1] = tessellate;
	output.edges[2] = tessellate;
	output.edges[3] = tessellate;

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

	//position = control point as output
	output.position = patch[pointId].position;
	output.tex = patch[pointId].tex;
	return output;
}