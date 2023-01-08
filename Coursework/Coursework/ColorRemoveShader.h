#pragma once
#include "BaseShader.h"

using namespace std;
using namespace DirectX;

class ColorRemoveShader : public BaseShader
{
public:
	struct bloomDataType {
		float remove;//amount removed from plain render to texture
		float threshold;//amount that needs to be visable
		XMFLOAT2 pad;
	};

	ColorRemoveShader(ID3D11Device* device, HWND hwnd);
	~ColorRemoveShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, float remove, float threshold);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	//Standard Matrices
	ID3D11Buffer* matrixBuffer;
	//used to remove non bloom colours
	ID3D11Buffer* bloomDataBuffer;
	//Used for converting texture to colour
	ID3D11SamplerState* sampleState;
};

