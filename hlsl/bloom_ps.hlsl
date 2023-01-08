// Texture pixel/fragment shader
// Basic fragment shader for rendering textured geometry

// Texture and sampler registers
Texture2D texture0 : register(t0);
Texture2D texture1 : register(t1);
SamplerState Sampler0 : register(s0);

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};


float4 main(InputType input) : SV_TARGET
{
	//sample the pixel color from the texture
	float4 textureColor = texture0.Sample(Sampler0, input.tex);
	//sample the pixel color from the blurred texture
	float4 blurredColor = texture1.Sample(Sampler0, input.tex);

	//add the blur colour to the normal texture
	return textureColor + blurredColor / 2;
}