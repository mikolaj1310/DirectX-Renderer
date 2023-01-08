Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

struct InputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct OutputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 sv : SV_Position;
};

OutputType main(InputType input)
{
	OutputType output;
	float4 colour = texture0.SampleLevel(sampler0, input.tex, 0);

	output.position = float4(input.position.x, input.position.y, input.position.zw);
	output.tex = input.tex;
	output.normal = input.normal;
	output.sv = float4(0, 0, 0, 0);

	return output;
}