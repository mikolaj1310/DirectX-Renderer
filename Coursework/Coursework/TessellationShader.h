// Light shader.h
// Basic single light shader setup
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class TessellationShader : public BaseShader
{

public:
	struct testType //for LOD
	{
		XMFLOAT3 camPos;
		float pad;
	};

	struct MatrixBufferType //default matrices
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

	struct LightBufferType //lights
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT4 lPosition;

		XMFLOAT4 sDiffuse;
		XMFLOAT4 sLPosition;

		XMFLOAT4 tDiffuse;
		XMFLOAT4 tDirection;
	};


	TessellationShader(ID3D11Device* device, HWND hwnd);
	~TessellationShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* color, XMFLOAT3 camPos, Light* light[3], ID3D11ShaderResourceView* depthMap[3]);

private:
	void initShader(const wchar_t* vsFilename, const wchar_t* psFilename);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* lodBuffer; //LOD buffer
	ID3D11Buffer* lightBuffer; //LightBuffers
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;
};
