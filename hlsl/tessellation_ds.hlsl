Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
	matrix sLightViewMatrix;
	matrix sLightProjectionMatrix;
	matrix tLightViewMatrix;
	matrix tLightProjectionMatrix;
};

struct ConstantOutputType
{
	float edges[4] : SV_TessFactor;
	float inside[2] : SV_InsideTessFactor;
};

struct InputType
{
	float2 tex : TEXCOORD4;
	float3 position : POSITION;
};

struct OutputType
{
	float2 tex : TEXCOORD4;
	float3 normal : NORMAL;
	float3 worldPosition : TEXCOORD3;
	float4 tLightViewPos : TEXCOORD2;
	float4 sLightViewPos : TEXCOORD1;
	float4 lightViewPos : TEXCOORD0;
	float4 position : SV_POSITION;
};

//Calculate light view matrix
float4 calculateLightView(float3 VertexPosition, matrix LightView, matrix LightProjection)
{
	float4 temp = mul(float4(VertexPosition, 1.0f), worldMatrix);
	temp = mul(temp, LightView);
	temp = mul(temp, LightProjection);

	return temp;
}

float3 calculateNormals(OutputType output)
{
	float space = 1.f / 200.f;

	//Calculate vetex postions
	float orgin = ((texture0.SampleLevel(sampler0, output.tex, 0.f))).r;
	float u = ((texture0.SampleLevel(sampler0, output.tex + float2(0.f, -space), 0.f)));
	float d = ((texture0.SampleLevel(sampler0, output.tex + float2(0.f, space), 0.f)));
	float l = ((texture0.SampleLevel(sampler0, output.tex + float2(-space, 0.f), 0.f)));
	float r = ((texture0.SampleLevel(sampler0, output.tex + float2(space, 0.f), 0.f)));

	//tangents and bitangents
	float3 tang = normalize(float3(2 * space, (r - l) * 3, 0.f));
	float3 bitang = normalize(float3(0.f, (d - u) * 3, (-2.f) * space));

	//Get the cross product
	float3 temp = cross(tang, bitang);
	//Times by worldmatrix and return
	temp = mul(temp, (float3x3) worldMatrix);
	return normalize(temp);
}

[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
	float3 vertexPosition;
	OutputType output;

	float3 v1 = lerp(patch[0].position, patch[1].position, uvwCoord.y);
	float3 v2 = lerp(patch[3].position, patch[2].position, uvwCoord.y);
	vertexPosition = lerp(v1, v2, uvwCoord.x);

	//calc texture pos on new surface
	float2 t1 = lerp(patch[0].tex, patch[1].tex, uvwCoord.y);
	float2 t2 = lerp(patch[3].tex, patch[2].tex, uvwCoord.y);
	output.tex = lerp(t1, t2, uvwCoord.x);

	//set vertex y position based on the height map colour value
	vertexPosition.y = texture0.SampleLevel(sampler0, output.tex, 0) * 4;

	output.normal = calculateNormals(output);

	//calculate new world position
	output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	//calulate light view position for lights
	output.lightViewPos = calculateLightView(vertexPosition, lightViewMatrix, lightProjectionMatrix);
	output.sLightViewPos = calculateLightView(vertexPosition, sLightViewMatrix, sLightProjectionMatrix);
	output.tLightViewPos = calculateLightView(vertexPosition, tLightViewMatrix, tLightProjectionMatrix);

	output.worldPosition = mul(vertexPosition, (float3x3)worldMatrix).xyz;
	return output;
}

