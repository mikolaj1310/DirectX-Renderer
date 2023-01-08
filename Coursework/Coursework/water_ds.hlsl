Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;

	//first light
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
	//second light
	matrix sLightViewMatrix;
	matrix sLightProjectionMatrix;
	//third light
	matrix tLightViewMatrix;
	matrix tLightProjectionMatrix;
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
	float2 tex : TEXCOORD4;
	float3 position : POSITION;
};

struct OutputType
{
	float2 tex : TEXCOORD4;
	float3 norm : NORMAL;
	float3 worldPosition : TEXCOORD3;
	float4 tLightViewPos : TEXCOORD2;
	float4 sLightViewPos : TEXCOORD1;
	float4 lightViewPos : TEXCOORD0;
	float4 position : SV_POSITION;
};

//return light view matrix
float4 calculateLightView(float3 VertexPosition, matrix LightView, matrix LightProjection)
{
	float4 temp = mul(float4(VertexPosition, 1.0f), worldMatrix);
	temp = mul(temp, LightView);
	temp = mul(temp, LightProjection);

	return temp;
}

[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
	float3 vertexPosition;
	OutputType output;

	float3 vert1 = lerp(patch[0].position, patch[1].position, uvwCoord.y);
	float3 vert2 = lerp(patch[3].position, patch[2].position, uvwCoord.y);
	vertexPosition = lerp(vert1, vert2, uvwCoord.x);

	//Calculates texture coordinates on a tesselated surface
	float2 tex1 = lerp(patch[0].tex, patch[1].tex, uvwCoord.y);
	float2 tex2 = lerp(patch[3].tex, patch[2].tex, uvwCoord.y);
	output.tex = lerp(tex1, tex2, uvwCoord.x);

	//Sets values needed to generate Waves Y position
	float l = length;   //length
	float w = 2 / l;    //Width
	float a = amper;    //Ampplatude
	float s = speed;    //Speed
	float d = distance; //Distance between waves
	float displacement = s * w;
	float currentTime = time;

	//get dot of xz
	float DotProd = dot(d, vertexPosition.xz);
	//get y position
	vertexPosition.y = (a * sin((DotProd * w) + (currentTime * displacement)) + 0.75);


	//calculate position of new vertex against world, view, and projection matrices.
	output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	//calculates wave normals 
	output.norm = float3(a * -cos((DotProd * w) + (currentTime * displacement)) + 0.75, 1, 0);
	output.norm = mul(output.norm, (float3x3) worldMatrix);
	output.norm = normalize(output.norm);

	//calculate lighting positions
	output.lightViewPos = calculateLightView(vertexPosition, lightViewMatrix, lightProjectionMatrix);
	output.sLightViewPos = calculateLightView(vertexPosition, sLightViewMatrix, sLightProjectionMatrix);
	output.tLightViewPos = calculateLightView(vertexPosition, tLightViewMatrix, tLightProjectionMatrix);

	//calculate world pos
	output.worldPosition = mul(vertexPosition, (float3x3) worldMatrix).xyz;

	return output;
}


