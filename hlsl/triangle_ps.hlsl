// Simple colour pixel shader

Texture2D texture0 : register(t0);
SamplerState Sampler0 : register(s0);

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};


float4 main(InputType input) : SV_TARGET
{
	//sample texture
	float4 textureColor = texture0.Sample(Sampler0, input.tex);

	//return colour
	return textureColor;
}