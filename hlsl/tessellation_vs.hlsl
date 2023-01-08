struct InputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct OutputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
	float4 sv : SV_Position;
};

OutputType main(InputType input)
{
	OutputType output;

	output.position = input.position;
	output.tex = input.tex;
	output.sv = float4(0, 0, 0, 0);

	return output;
}
