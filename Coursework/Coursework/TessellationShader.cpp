// tessellation shader.cpp
#include "tessellationshader.h"


TessellationShader::TessellationShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"tessellation_vs.cso", L"tessellation_hs.cso", L"tessellation_ds.cso", L"tessellation_ps.cso");
}


TessellationShader::~TessellationShader()
{
	//Release texture sampler
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}
	//release matices buffer
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}
	//Release LOD buffer
	if (lodBuffer)
	{
		lodBuffer->Release();
		lodBuffer = 0;
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

void TessellationShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
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

	//LOD buffer
	D3D11_BUFFER_DESC lodBufferDesc;
	lodBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lodBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	lodBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lodBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lodBufferDesc.MiscFlags = 0;
	lodBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lodBufferDesc, NULL, &lodBuffer);

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

void TessellationShader::initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename)
{
	// InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
	initShader(vsFilename, psFilename);

	// Load other required shaders.
	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}


void TessellationShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* color, XMFLOAT3 camPos, Light* light[3], ID3D11ShaderResourceView* depthMap[3])
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);
	XMMATRIX lightView = XMMatrixTranspose(light[0]->getViewMatrix());
	XMMATRIX lightProjection = XMMatrixTranspose(light[0]->getOrthoMatrix());
	XMMATRIX sLightView = XMMatrixTranspose(light[1]->getViewMatrix());
	XMMATRIX sLightProjection = XMMatrixTranspose(light[1]->getOrthoMatrix());
	XMMATRIX tLightView = XMMatrixTranspose(light[2]->getViewMatrix());
	XMMATRIX tLightProjection = XMMatrixTranspose(light[2]->getOrthoMatrix());

	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	dataPtr->lightView = lightView;
	dataPtr->lightProjection = lightProjection;
	dataPtr->sLightView = sLightView;
	dataPtr->sLightProjection = sLightProjection;
	dataPtr->tLightView = tLightView;
	dataPtr->tLightProjection = tLightProjection;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);

	deviceContext->DSSetShaderResources(0, 1, &texture);
	deviceContext->DSSetSamplers(0, 1, &sampleState);

	//send LOD buffer to hull shader
	result = deviceContext->Map(lodBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	testType* testPtr = (testType*)mappedResource.pData;
	testPtr->camPos = camPos;
	testPtr->pad = float();
	deviceContext->Unmap(lodBuffer, 0);
	deviceContext->HSSetConstantBuffers(0, 1, &lodBuffer);

	//send light desriptor to pixel shader
	result = deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	LightBufferType* lightPtr = (LightBufferType*)mappedResource.pData;
	
	//Lights
	lightPtr->ambient = light[0]->getAmbientColour();
	lightPtr->diffuse = light[0]->getDiffuseColour();
	lightPtr->lPosition = XMFLOAT4(light[0]->getPosition().x, light[0]->getPosition().y, light[0]->getPosition().z, 0);

	lightPtr->sDiffuse = light[1]->getDiffuseColour();
	lightPtr->sLPosition = XMFLOAT4(light[1]->getPosition().x, light[1]->getPosition().y, light[1]->getPosition().z, 0);

	lightPtr->tDiffuse = light[2]->getDiffuseColour();
	lightPtr->tDirection = XMFLOAT4(light[2]->getDirection().x, light[2]->getDirection().y, light[2]->getDirection().z, 0);
	deviceContext->Unmap(lightBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

	//send shader texture and sampler to pixel shader
	deviceContext->PSSetShaderResources(0, 1, &color);
	deviceContext->PSSetShaderResources(1, 1, &depthMap[0]);
	deviceContext->PSSetShaderResources(2, 1, &depthMap[1]);
	deviceContext->PSSetShaderResources(3, 1, &depthMap[2]);

	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->PSSetSamplers(1, 1, &sampleStateShadow);
}


