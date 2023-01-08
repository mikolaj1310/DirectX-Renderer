#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class DepthShader : public BaseShader
{
public:
	//LOD buffer
	struct testType
	{
		float tessFactor;
		XMFLOAT3 camPos;
	};
	//waves buffer
	struct deltaType
	{
		float time;
		float amper;
		float length;
		float speed;
		float distance;
		XMFLOAT3 padding;
	};
	//grass buffer
	struct grassBufferType
	{
		int slider;
		XMFLOAT3 camPos;

		float time;
		float speed;
		float limit;
		float lHight;

		float uHeight;
		XMFLOAT3 padding;
	};

	DepthShader(ID3D11Device* device, HWND hwnd, int type);
	~DepthShader();

	//Set shader parameters for ground
	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture);
	//Set shader parameters for waves
	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, float deltaTime, float amper, float length, float speed, float distance, XMFLOAT3 camPos);
	//Set shader for grass 
	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* displacement, XMFLOAT4 slide_Cam, XMFLOAT3 tsl, float slide, float lHeight, float uHeight);

private:
	void initShader(const wchar_t* vsFilename, const wchar_t* psFilename);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);
	void initShader(const wchar_t* vsFilename, const wchar_t* gsFilename, const wchar_t* psFilename);

	//Martrix buffer
	ID3D11Buffer* matrixBuffer;
	//Wave Buffer
	ID3D11Buffer* deltaBuffer;
	//Texture sampler
	ID3D11SamplerState* sampleState;
	//Grass bufffer
	ID3D11Buffer* grassMoveBuffer;

	int type = 0;
};

