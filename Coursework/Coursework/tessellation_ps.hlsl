Texture2D texture0 : register(t0);
Texture2D depthMap : register(t1);
Texture2D sDepthMap : register(t2);
Texture2D tDepthMap : register(t3);

SamplerState sampler0 : register(s0);
SamplerState shadowSampler : register(s1);

cbuffer lightBuffer : register(b0)
{
	float4 ambient;
	float4 diffuse;
	float4 lPosition;

	float4 sDiffuse;
	float4 sLPosition;

	float4 tDiffuse;
	float4 tDirection;

};

struct InputType
{
	float2 tex : TEXCOORD4;
	float3 normal : NORMAL;
	float3 worldPosition : TEXCOORD3;
	float4 tLightViewPos : TEXCOORD2;
	float4 sLightViewPos : TEXCOORD1;
	float4 lightViewPos : TEXCOORD0;
	float4 position : SV_POSITION;
};

//calculate lighting intensity then combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
	float intensity = saturate(dot(normal, lightDirection));
	float4 colour = saturate(diffuse * intensity);
	return colour;
}

//calculate lighting intensity then combine with colour
float4 calculateLightingA(float3 lightDirection, InputType input, float4 ldiffuse, float3 position)
{
	float intensity = saturate(dot(input.normal, lightDirection));
	float4 colour = saturate(ldiffuse * intensity);

	float d = length(position - input.worldPosition);
	float q = 0.0f;
	float l = 0.125f;
	float c = 0.5f;
	float attributeValue = 1 / ((c + (l * d)) + (q * pow(d, 2)));

	colour *= attributeValue;
	colour = saturate(colour);
	return colour;
}

//check if geometry is inside shadow map
bool hasDepthData(float2 uv)
{
	if (uv.x < 0.f || uv.x > 1.f || uv.y < 0.f || uv.y > 1.f)
	{
		return false;
	}
	return true;
}

//code from lab
float2 getProjectiveCoords(float4 lightViewPosition)
{
	// Calculate the projected texture coordinates.
	float2 projTex = lightViewPosition.xy / lightViewPosition.w;
	projTex *= float2(0.5, -0.5);
	projTex += float2(0.5f, 0.5f);
	return projTex;
}

//code from lab
bool isInShadow(Texture2D sMap, float2 uv, float4 lightViewPosition, float bias)
{
	// Sample the shadow map (get depth of geometry)
	float depthValue = sMap.Sample(shadowSampler, uv).r;
	// Calculate the depth from the light.
	float lightDepthValue = lightViewPosition.z / lightViewPosition.w;
	lightDepthValue -= bias;

	if (lightDepthValue < depthValue)
	{
		return false;
	}
	return true;
}


float4 main(InputType input) : SV_TARGET
{
	//sample texture, calculate light, return light and texture
	float3 lightVector = normalize(lPosition.xyz - input.worldPosition);
	float3 sLightVector = normalize(sLPosition.xyz - input.worldPosition);

	float shadowMapBias = 0.020f;
	float4 colour = float4(0.f, 0.f, 0.f, 1.f);
	float4 sColour = float4(0.f, 0.f, 0.f, 1.f);
	float4 tColour = float4(0.f, 0.f, 0.f, 1.f);
	float4 textureColour = texture0.Sample(sampler0, input.tex);

	//calculate projected coordinates
	float2 pTexCoord = getProjectiveCoords(input.lightViewPos);
	float2 pSTexCoord = getProjectiveCoords(input.sLightViewPos);
	float2 pTTexCoord = getProjectiveCoords(input.tLightViewPos);

	//check if in shadow or not
	if (hasDepthData(pTexCoord))
	{
		if (!isInShadow(depthMap, pTexCoord, input.lightViewPos, shadowMapBias))
		{
			//if not in shadow, light
			colour = calculateLightingA(lightVector, input, diffuse, lPosition.xyz);
		}
	}
	//check if in shadow
   if (hasDepthData(pSTexCoord))
   {
	   if (!isInShadow(sDepthMap, pSTexCoord, input.sLightViewPos, shadowMapBias))
	   {
		   //if not in shadow, light
		   sColour = calculateLightingA(sLightVector, input, sDiffuse, sLPosition.xyz);
	   }
   }
   //check if in shadow
   if (hasDepthData(pTTexCoord))
   {
	   if (!isInShadow(sDepthMap, pTTexCoord, input.tLightViewPos, shadowMapBias))
	   {
		   //if not in shadow, light
		   tColour = calculateLighting(-tDirection.xyz, input.normal, tDiffuse);
	   }
   }

   //saturate lights
   colour = saturate(colour + sColour + tColour + ambient);
   //apply texture to light
   return saturate(colour) * textureColour;
}