#include "DepthShader.h"

DepthShader::DepthShader(ID3D11Device* device, HWND hwnd, int typeIndex) : BaseShader(device, hwnd)
{
	//Check what hlsl files we need
	type = typeIndex;
	switch (type)
	{
	case 0:	//Ground Files
		initShader(L"depth_vs.cso", L"depth_hs.cso", L"depthGround_ds.cso", L"depth_ps.cso");
		break;
	case 1:	//Water files
		initShader(L"depth_vs.cso", L"depth_hs.cso", L"depthWater_ds.cso", L"depth_ps.cso");
		break;
	case 2:	//grass files
		initShader(L"depth_vs.cso", L"depthGrass_gs.cso", L"depth_ps.cso");
		break;
	}
}

DepthShader::~DepthShader()
{
	//Release Texture sampler
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}
	//Release matrix buffer
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}
	//Release grass buffer
	if (grassMoveBuffer)
	{
		grassMoveBuffer->Release();
		grassMoveBuffer = 0;
	}
	//Release water buffer
	if (deltaBuffer)
	{
		deltaBuffer->Release();
		deltaBuffer = 0;
	}
	//Release layout
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void DepthShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	//delta time buffer
	D3D11_BUFFER_DESC deltaBufferDesc;
	deltaBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	deltaBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	deltaBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	deltaBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	deltaBufferDesc.MiscFlags = 0;
	deltaBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&deltaBufferDesc, NULL, &deltaBuffer);

	D3D11_SAMPLER_DESC samplerDesc;
	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	D3D11_BUFFER_DESC grassBufferDesc;
	grassBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	grassBufferDesc.ByteWidth = sizeof(grassBufferType);
	grassBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	grassBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	grassBufferDesc.MiscFlags = 0;
	grassBufferDesc.StructureByteStride = 0;
	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	renderer->CreateBuffer(&grassBufferDesc, NULL, &grassMoveBuffer);
}

void DepthShader::initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename)
{
	// InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
	initShader(vsFilename, psFilename);

	// Load other required shaders.
	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}

void DepthShader::initShader(const wchar_t* vsFilename, const wchar_t* gsFilename, const wchar_t* psFilename)
{
	// InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
	initShader(vsFilename, psFilename);

	// Load other required shaders.
	loadGeometryShader(gsFilename);
}

void DepthShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(world);
	XMMATRIX tview = XMMatrixTranspose(view);
	XMMATRIX tproj = XMMatrixTranspose(projection);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);

	deviceContext->DSSetShaderResources(0, 1, &texture);
	deviceContext->DSSetSamplers(0, 1, &sampleState);
}

void DepthShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, float deltaTime, float amper, float length, float speed, float distance, XMFLOAT3 camPos)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(world);
	XMMATRIX tview = XMMatrixTranspose(view);
	XMMATRIX tproj = XMMatrixTranspose(projection);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);

	//Wave info sent to domian shader
	result = deviceContext->Map(deltaBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	deltaType* timePtr = (deltaType*)mappedResource.pData;
	timePtr->time = deltaTime;// worldMatrix;
	timePtr->amper = amper;
	timePtr->length = length;
	timePtr->speed = speed;
	timePtr->distance = distance;
	timePtr->padding = XMFLOAT3();
	deviceContext->Unmap(deltaBuffer, 0);
	deviceContext->DSSetConstantBuffers(1, 1, &deltaBuffer);
}

void DepthShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* displacement, XMFLOAT4 slide_Cam, XMFLOAT3 tsl, float slide, float lHeight, float uHeight)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(world);
	XMMATRIX tview = XMMatrixTranspose(view);
	XMMATRIX tproj = XMMatrixTranspose(projection);

	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->GSSetConstantBuffers(0, 1, &matrixBuffer);

	//Grass buffer sent to geometery shader
	deviceContext->Map(grassMoveBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	grassBufferType* grassPtr;
	grassPtr = (grassBufferType*)mappedResource.pData;
	grassPtr->slider = slide;// worldMatrix;
	grassPtr->camPos = XMFLOAT3(slide_Cam.y, slide_Cam.z, slide_Cam.w);
	grassPtr->time = tsl.x;
	grassPtr->speed = tsl.y;
	grassPtr->limit = tsl.z;
	grassPtr->lHight = lHeight;
	grassPtr->uHeight = uHeight;
	grassPtr->padding = XMFLOAT3();
	deviceContext->Unmap(grassMoveBuffer, 0);
	deviceContext->GSSetConstantBuffers(1, 1, &grassMoveBuffer);

	// Set shader texture resource in the Geometry shader.
	deviceContext->GSSetShaderResources(0, 1, &displacement);
	deviceContext->GSSetSamplers(0, 1, &sampleState);
}
