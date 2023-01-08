Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer ScreenSizeBuffer : register(b0)
{
	float screenWidth;
	float3 padding;
};

struct InputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
	float w0;
	float w1;
	float w2;
	float w3;
	float w4;
	float4 colour;

	// Create the weights that each neighbor pixel will contribute to the blur. 
	w0 = 0.38292;
	w1 = 0.24173;
	w2 = 0.06059;
	w3 = 0.00597;
	w4 = 0.00022;

	// Initialize the colour to black.
	colour = float4(0.0f, 0.0f, 0.0f, 0.0f);

	//set the alpha channel
	colour.a = 1.0f;

	//pixel size
	float pixelSize = 1.0f / screenWidth;

	//add the horizontal value to the colour depending on weighting
	colour += shaderTexture.Sample(SampleType, input.tex + float2(pixelSize * -8.0f, 0.0f)) * w4;
	colour += shaderTexture.Sample(SampleType, input.tex + float2(pixelSize * -6.0f, 0.0f)) * w3;
	colour += shaderTexture.Sample(SampleType, input.tex + float2(pixelSize * -4.0f, 0.0f)) * w2;
	colour += shaderTexture.Sample(SampleType, input.tex + float2(pixelSize * -2.0f, 0.0f)) * w1;
	colour += shaderTexture.Sample(SampleType, input.tex) * w0;
	colour += shaderTexture.Sample(SampleType, input.tex + float2(pixelSize * 2.0f, 0.0f)) * w1;
	colour += shaderTexture.Sample(SampleType, input.tex + float2(pixelSize * 4.0f, 0.0f)) * w2;
	colour += shaderTexture.Sample(SampleType, input.tex + float2(pixelSize * 6.0f, 0.0f)) * w3;
	colour += shaderTexture.Sample(SampleType, input.tex + float2(pixelSize * 8.0f, 0.0f)) * w4;


	return colour;
}
