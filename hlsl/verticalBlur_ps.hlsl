Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer ScreenSizeBuffer : register(b0)
{
	float screenHeight;
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
	float w1; float4 colour;
	float w2;
	float w3;// Create the weights that each neighbor pixel will contribute to the blur.
	float w4; 
	
	w0 = 0.48292;
	w1 = 0.34173;
	w2 = 0.13059;
	w3 = 0.02534;
	w4 = 0.01940;

	// Initialize the colour to black.
	colour = float4(0.0f, 0.0f, 0.0f, 0.0f);

	//set the alpha channel
	colour.a = 1.0f;

	float pixelSize = 1.0f / screenHeight;
	// Add the vertical pixels to the colour by the specific weight of each.
	colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, pixelSize * -8.0f)) * w4;
	colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, pixelSize * -6.0f)) * w3;
	colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, pixelSize * -4.0f)) * w2;
	colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, pixelSize * -2.0f)) * w1;
	colour += shaderTexture.Sample(SampleType, input.tex) * w0;
	colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, pixelSize * 2.0f)) * w1;
	colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, pixelSize * 4.0f)) * w2;
	colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, pixelSize * 6.0f)) * w3;
	colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, pixelSize * 8.0f)) * w4;



	return colour;
}

