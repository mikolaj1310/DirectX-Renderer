//tessellation domain shader
cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer dtbuffer : register(b1)
{
	float time;
	float amper;
	float length;
	float speed;
	float distance;
};

struct ConstantOutputType
{
	float edges[4] : SV_TessFactor;
	float inside[2] : SV_InsideTessFactor;
};


struct InputType
{
	float3 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float4 depthPosition : TEXCOORD1;
};

[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
	float3 vertexPosition;
	OutputType output;

	//calculate vert pos on new surface
	float3 v1 = lerp(patch[0].position, patch[1].position, uvwCoord.y);
	float3 v2 = lerp(patch[3].position, patch[2].position, uvwCoord.y);
	vertexPosition = lerp(v1, v2, uvwCoord.x);

	//calculate texture position
	float2 t1 = lerp(patch[0].tex, patch[1].tex, uvwCoord.y);
	float2 t2 = lerp(patch[3].tex, patch[2].tex, uvwCoord.y);
	float2 tex = lerp(t1, t2, uvwCoord.x);

	//values passed in from wave calculation
	float l = length;
	float w = 2 / l;
	float a = amper;
	float s = speed;
	float d = distance;
	float displacement = s * (2 / l);
	float currentTime = time;

	//calculate y vertex position of the wave
	float dotProd = dot(d, vertexPosition.xz);
	vertexPosition.y = (a * sin((dotProd * w) + (currentTime * displacement)) + 0.75);

	//calculate new position
	output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.depthPosition = output.position;

	return output;
}