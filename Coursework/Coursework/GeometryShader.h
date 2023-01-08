#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class GeometryShader : public BaseShader
{

public:

	struct grassBufferType
	{
		XMFLOAT3 camPos;
		float time; //delta time
		float speed; // grass sway speed
		float limit; //amount the grass can move
		float lHight; //lower threshold for grass
		float uHeight; //upper threshold for grass
	};

	GeometryShader(ID3D11Device* device, HWND hwnd);
	~GeometryShader();

	//input texture for grass and height map
	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* displacement, XMFLOAT4 cam_time, XMFLOAT4 speedandlimits);

private:
	void initShader(const wchar_t* vsFilename, const wchar_t* psFilename);
	void initShader(const wchar_t* vsFilename, const wchar_t* gsFilename, const wchar_t* psFilename);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* grassBuffer;
};
