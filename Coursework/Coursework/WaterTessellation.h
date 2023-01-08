#pragma once
#include "DXF.h"

using namespace std;
using namespace DirectX;

class WaterTessellation : public BaseShader
{

public:
	//camera buffer to change level of tesselation
	struct tessType
	{
		XMFLOAT3 camPos;
		float pad;
	};
	//delta buffer handles data needed for generating waves
	struct deltaType
	{
		float time;
		float amper;
		float length;
		float speed;
		float distance;
		XMFLOAT3 padding;
	};
	//Light buffer for all 3 lights
	struct LightBufferType
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 lPosition;

		XMFLOAT4 sAmbient;
		XMFLOAT4 sDiffuse;
		XMFLOAT4 sLPosition;

		XMFLOAT4 tAmbient;
		XMFLOAT4 tDiffuse;
		XMFLOAT4 tDirection;
		XMFLOAT4 tLPosition;
	};
	//Standard matrix buffer and a light view and projection for all lights
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView;
		XMMATRIX lightProjection;
		XMMATRIX sLightView;
		XMMATRIX sLightProjection;
		XMMATRIX tLightView;
		XMMATRIX tLightProjection;
	};

	WaterTessellation(ID3D11Device* device, HWND hwnd);
	~WaterTessellation();

	//Takes in array of lights and depthmaps, DALS = Delta time, Amplitude, Lenght and Speed
	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, XMFLOAT4 DALS, float distance, XMFLOAT3 camPos, ID3D11ShaderResourceView* depthMap[3], Light* light[3]);

private:
	void initShader(const wchar_t* vsFilename, const wchar_t* psFilename);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* lodBuffer; //LOD Buffer
	ID3D11Buffer* deltaBuffer; //water wave buffer
	ID3D11Buffer* lightBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;

};

