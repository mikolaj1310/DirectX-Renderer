// geometry shader.cpp
#include "geometryshader.h"

GeometryShader::GeometryShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"triangle_vs.cso", L"triangle_gs.cso", L"triangle_ps.cso");
}

GeometryShader::~GeometryShader()
{
	// Release the sampler state.
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}

	// Release the matrix constant buffer.
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	// Release the matrix constant buffer.
	if (grassBuffer)
	{
		grassBuffer->Release();
		grassBuffer = 0;
	}

	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	BaseShader::~BaseShader();
}

void GeometryShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	// Load shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//texture sampler state.
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	D3D11_BUFFER_DESC grassBufferDesc;
	grassBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	grassBufferDesc.ByteWidth = sizeof(grassBufferType);
	grassBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	grassBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	grassBufferDesc.MiscFlags = 0;
	grassBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&grassBufferDesc, NULL, &grassBuffer);
}

void GeometryShader::initShader(const wchar_t* vsFilename, const wchar_t* gsFilename, const wchar_t* psFilename)
{
	//loads vertex and pixel shaders and sets up buffers for them
	initShader(vsFilename, psFilename);

	//Load other shaders
	loadGeometryShader(gsFilename);
}


void GeometryShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* displacement, XMFLOAT4 cam_time, XMFLOAT4 speedandlimits)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);

	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->GSSetConstantBuffers(0, 1, &matrixBuffer);

	//Passes values to geometry shader
	deviceContext->Map(grassBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	grassBufferType* grassPtr;
	grassPtr = (grassBufferType*)mappedResource.pData;
	grassPtr->camPos = XMFLOAT3(cam_time.x, cam_time.y, cam_time.z);
	grassPtr->time = cam_time.w;
	grassPtr->speed = speedandlimits.x;
	grassPtr->limit = speedandlimits.y;
	grassPtr->lHight = speedandlimits.z;
	grassPtr->uHeight = speedandlimits.w;
	deviceContext->Unmap(grassBuffer, 0);
	deviceContext->GSSetConstantBuffers(1, 1, &grassBuffer);

	deviceContext->VSSetShaderResources(0, 1, &displacement);
	deviceContext->VSSetSamplers(0, 1, &sampleState);

	deviceContext->GSSetShaderResources(0, 1, &displacement);
	deviceContext->GSSetSamplers(0, 1, &sampleState);

	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetSamplers(0, 1, &sampleState);

}
