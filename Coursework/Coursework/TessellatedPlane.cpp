#include "TessellatedPlane.h"
// Quad Mesh
// Simple unit quad mesh with texture coordinates and normals.


// Initialise buffers and lad texture.
TessellatedPlane::TessellatedPlane(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	initBuffers(device);

}

// Release resources.
TessellatedPlane::~TessellatedPlane()
{
	// Run parent deconstructor
	BaseMesh::~BaseMesh();
}

// Build quad mesh.
void TessellatedPlane::initBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	float positionX, positionZ, u, v, increment;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	int resolution = 100;

	vertexCount = (resolution - 1) * (resolution - 1) * 6;

	indexCount = vertexCount;

	int index = 0;
	// UV coords.
	u = 0;
	v = 0;
	increment = 1.f / resolution;
	vertices = new VertexType[vertexCount];
	indices = new unsigned long[indexCount];

	for (int j = 0; j < resolution; j++)
	{
		for (int i = 0; i < resolution; i++)
		{
			positionX = (float)(i);
			positionZ = (float)(j);

			// Load the vertex array with data.
			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);  // Bottom left.
			vertices[index].texture = XMFLOAT2(u, v);//0.0f, 1.0f);
			vertices[index].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			indices[index] = index;
			index++;

			positionX = (float)(i + 1);
			positionZ = (float)(j);

			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);  // Top left.
			vertices[index].texture = XMFLOAT2(u + increment, v);//0.0f, 0.0f);
			vertices[index].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			indices[index] = index;
			index++;

			positionX = (float)(i + 1);
			positionZ = (float)(j + 1);

			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);  // Top right.
			vertices[index].texture = XMFLOAT2(u + increment, v + increment);//1.0f, 0.0f);
			vertices[index].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			indices[index] = index;
			index++;

			positionX = (float)(i);
			positionZ = (float)(j + 1);

			vertices[index].position = XMFLOAT3(positionX, 0.0f, positionZ);  // Bottom right.
			vertices[index].texture = XMFLOAT2(u, v + increment);// 1.0f, 1.0f);
			vertices[index].normal = XMFLOAT3(0.0f, 1.0f, 00);
			indices[index] = index;
			index++;

			u += increment;
		}
		u = 0;
		v += increment;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// Now create the vertex buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	// Create the index buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;
}
void TessellatedPlane::sendData(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// Set the type of primitive that should be rendered from this vertex buffer, in this case control patch for tessellation.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
}