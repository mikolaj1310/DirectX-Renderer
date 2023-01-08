#include "TessellationPlane.h"

TessellationPlane::TessellationPlane(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int lresolution)
{
	resolution = lresolution;
	initBuffers(device);
}

TessellationPlane::~TessellationPlane()
{
	// Run parent deconstructor
	BaseMesh::~BaseMesh();
}

void TessellationPlane::initBuffers(ID3D11Device* device)
{
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	unsigned long* indices;
	VertexType* vertices;
	float posX, posZ, u = 0, v = 0, increment = 1.0f / resolution;

	// Calculate vertex count in terrain mesh
	vertexCount = (resolution - 1) * (resolution - 1) * 4;

	indexCount = vertexCount;
	vertices = new VertexType[vertexCount];
	indices = new unsigned long[indexCount];
	

	int index = 0;
	for (int j = 0; j < (resolution - 1); j++)
	{
		for (int i = 0; i < (resolution - 1); i++)
		{
			// lower left
			posX = (float)(i);
			posZ = (float)(j + 1);
			vertices[index].position = XMFLOAT3(posX, 0.0f, posZ);
			vertices[index].texture = XMFLOAT2(u, v + increment);
			vertices[index].normal = XMFLOAT3(0.0, 1.0, 0.0);
			indices[index] = index;
			index++;
			// Upper left.
			posX = (float)i;
			posZ = (float)(j);
			vertices[index].position = XMFLOAT3(posX, 0.0f, posZ);
			vertices[index].texture = XMFLOAT2(u, v);
			vertices[index].normal = XMFLOAT3(0.0, 1.0, 0.0);
			indices[index] = index;
			index++;

			// Bottom right
			posX = (float)(i + 1);
			posZ = (float)(j);
			vertices[index].position = XMFLOAT3(posX, 0.0f, posZ);
			vertices[index].texture = XMFLOAT2(u + increment, v);
			vertices[index].normal = XMFLOAT3(0.0, 1.0, 0.0);
			indices[index] = index;
			index++;

			// Upper right.
			posX = (float)(i + 1);
			posZ = (float)(j + 1);
			vertices[index].position = XMFLOAT3(posX, 0.0f, posZ);
			vertices[index].texture = XMFLOAT2(u + increment, v + increment);
			vertices[index].normal = XMFLOAT3(0.0, 1.0, 0.0);
			indices[index] = index;
			index++;

			u += increment;
		}
		u = 0;
		v += increment;
	}

	//vertex buffer desc
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//pointer to vertex data for
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	//index buffer desc
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	//pointer to index data
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	//index buffer
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	//clean up arrays
	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;
}

void TessellationPlane::sendData(ID3D11DeviceContext * deviceContext, D3D_PRIMITIVE_TOPOLOGY top)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(top);
}

