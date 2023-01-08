#include "WaterTessellation.h"

// tessellation shader.cpp
#include "tessellationshader.h"


WaterTessellation::WaterTessellation(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	//Reuse base tesselation vs, and hs them use our own water ds and ps
	initShader(L"tessellation_vs.cso", L"tessellation_hs.cso", L"water_ds.cso", L"water_ps.cso");
}


WaterTessellation::~WaterTessellation()
{
	//Release normal sampler
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}
	//release shadow sampler
	if (sampleStateShadow)
	{
		sampleStateShadow->Release();
		sampleStateShadow = 0;
	}
	//Release test buffer
	if (lodBuffer)
	{
		lodBuffer->Release();
		lodBuffer = 0;
	}
	//Release delta buffer
	if (deltaBuffer)
	{
		deltaBuffer->Release();
		deltaBuffer = 0;
	}
	//Release light buffer
	if (lightBuffer)
	{
		lightBuffer->Release();
		lightBuffer = 0;
	}
	//Release Layout
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}

void WaterTessellation::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
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

	//Camera buffer
	D3D11_BUFFER_DESC testBufferDesc;
	testBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	testBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	testBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	testBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	testBufferDesc.MiscFlags = 0;
	testBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&testBufferDesc, NULL, &lodBuffer);

	//delta time buffer
	D3D11_BUFFER_DESC deltaBufferDesc;
	deltaBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	deltaBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	deltaBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	deltaBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	deltaBufferDesc.MiscFlags = 0;
	deltaBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&deltaBufferDesc, NULL, &deltaBuffer);

	//Light buffer
	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);

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

	// Sampler for shadow map sampling.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	renderer->CreateSamplerState(&samplerDesc, &sampleStateShadow);
}

void WaterTessellation::initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename)
{
	// InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
	initShader(vsFilename, psFilename);

	// Load other required shaders.
	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}


void WaterTessellation::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, XMFLOAT4 DALS, float distance, XMFLOAT3 camPos, ID3D11ShaderResourceView* depthMap[3], Light* light[3])
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(world);
	XMMATRIX tview = XMMatrixTranspose(view);
	XMMATRIX tproj = XMMatrixTranspose(projection);
	//First Lights
	XMMATRIX lightView = XMMatrixTranspose(light[0]->getViewMatrix());
	XMMATRIX lightProjection = XMMatrixTranspose(light[0]->getOrthoMatrix());
	//Second Light
	XMMATRIX sLightView = XMMatrixTranspose(light[1]->getViewMatrix());
	XMMATRIX sLightProjection = XMMatrixTranspose(light[1]->getOrthoMatrix());
	//Third Light
	XMMATRIX tLightView = XMMatrixTranspose(light[2]->getViewMatrix());
	XMMATRIX tLightProjection = XMMatrixTranspose(light[2]->getOrthoMatrix());

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	dataPtr->lightView = lightView;				 //First light
	dataPtr->lightProjection = lightProjection;	 //First light
	dataPtr->sLightView = sLightView;			 //second light
	dataPtr->sLightProjection = sLightProjection;//second light
	dataPtr->tLightView = tLightView;			 //Third light
	dataPtr->tLightProjection = tLightProjection;//Third light
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);

	//Water data buffer
	result = deviceContext->Map(deltaBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	deltaType* timePtr = (deltaType*)mappedResource.pData;
	timePtr->time = DALS.x;// worldMatrix;
	timePtr->amper = DALS.y;
	timePtr->length = DALS.z;
	timePtr->speed = DALS.w;
	timePtr->distance = distance;
	timePtr->padding = XMFLOAT3();
	deviceContext->Unmap(deltaBuffer, 0);
	deviceContext->DSSetConstantBuffers(1, 1, &deltaBuffer);

	//LOD buffer
	result = deviceContext->Map(lodBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	tessType* tessPtr = (tessType*)mappedResource.pData;
	tessPtr->camPos = camPos;
	tessPtr->pad = float();// worldMatrix;
	deviceContext->Unmap(lodBuffer, 0);
	deviceContext->HSSetConstantBuffers(0, 1, &lodBuffer);

	//Fisrt light info
	result = deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	LightBufferType* lightPtr = (LightBufferType*)mappedResource.pData;
	lightPtr->ambient = light[0]->getAmbientColour();// worldMatrix;
	lightPtr->diffuse = light[0]->getDiffuseColour();
	lightPtr->lPosition = XMFLOAT4(light[0]->getPosition().x, light[0]->getPosition().y, light[0]->getPosition().z, 0);

	//Second Light Info
	lightPtr->sAmbient = light[1]->getAmbientColour();
	lightPtr->sDiffuse = light[1]->getDiffuseColour();
	lightPtr->sLPosition = XMFLOAT4(light[1]->getPosition().x, light[1]->getPosition().y, light[1]->getPosition().z, 0);

	//Third Light info
	lightPtr->tAmbient = light[2]->getAmbientColour();
	lightPtr->tDiffuse = light[2]->getDiffuseColour();
	lightPtr->tDirection = XMFLOAT4(light[2]->getDirection().x, light[2]->getDirection().y, light[2]->getDirection().z, 0);
	lightPtr->tLPosition = XMFLOAT4(light[2]->getPosition().x, light[2]->getPosition().y, light[2]->getPosition().z, 0);
	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

	//Pass texture, depthMaps and samplers to Pixel shader
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetShaderResources(1, 1, &depthMap[0]);
	deviceContext->PSSetShaderResources(2, 1, &depthMap[1]);
	deviceContext->PSSetShaderResources(3, 1, &depthMap[2]);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetSamplers(1, 1, &sampleStateShadow);
}