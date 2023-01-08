// Texture pixel/fragment shader
// Basic fragment shader for rendering textured geometry

// Texture and sampler registers
Texture2D texture0 : register(t0);
SamplerState Sampler0 : register(s0);

cbuffer bloomDataBuffer : register(b0)
{
	float remove;
	float threshold;
	float2 pad;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};


float4 main(InputType input) : SV_TARGET
{
	// Sample the pixel color from the texture
	float4 textureColor = texture0.Sample(Sampler0, input.tex);
	//remove at set amount from the base colour
	float rem = remove;
	float4 thresh2 = clamp(textureColor - float4(rem, rem, rem, rem), 0, 1);
	//calculate average colour
	float average = thresh2.x + thresh2.y + thresh2.x / 3;
	//Filter out background colour
	if (textureColor.x == 0.39f && textureColor.y == 0.58f && textureColor.z == 0.92f)
		return float4(0.0f, 0.0f, 0.0f, 0.0f);
	if (average < threshold)
		return float4(0, 0, 0, 0);
	else
		return thresh2;
}