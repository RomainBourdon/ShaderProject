#include "TessellationShader.h"


TessellationShader::TessellationShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"Tessellation_vs.cso", L"Tessellation_hs.cso", L"Tessellation_ds.cso", L"Tessellation_ps.cso");
}


TessellationShader::~TessellationShader()
{
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}
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
	D3D11_BUFFER_DESC factorBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;

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

	factorBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	factorBufferDesc.ByteWidth = sizeof(const_edgesBufferType);
	factorBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	factorBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	factorBufferDesc.MiscFlags = 0;
	factorBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&factorBufferDesc, NULL, &factorBuffer);

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

	// Setup light buffer
// Setup the description of the light dynamic constant buffer that is in the pixel shader.
// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);

}

void TessellationShader::initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename)
{
	// InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
	initShader(vsFilename, psFilename);

	// Load other required shaders.
	loadHullShader(hsFilename);
	loadDomainShader(dsFilename);
}


void TessellationShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, float time, XMFLOAT3 camera, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* texture2, Light* light, Light* light1, Light* light2, Light* light3, ID3D11ShaderResourceView* shadowmap)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Transpose the matrices to prepare them for the shader.
	XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
	XMMATRIX tview = XMMatrixTranspose(viewMatrix);
	XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);
	XMMATRIX tLightViewMatrix = XMMatrixTranspose(light->getViewMatrix());
	XMMATRIX tLightProjectionMatrix = XMMatrixTranspose(light->getOrthoMatrix());

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	MatrixBufferType* dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	dataPtr->lightView = tLightViewMatrix;
	dataPtr->lightProjection = tLightProjectionMatrix;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	const_edgesBufferType* facPtr;
	deviceContext->Map(factorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	facPtr = (const_edgesBufferType*)mappedResource.pData;
	facPtr->time = time;
	facPtr->camera = camera;
	deviceContext->Unmap(factorBuffer, 0);
	deviceContext->HSSetConstantBuffers(0, 1, &factorBuffer);
	deviceContext->DSSetConstantBuffers(1, 1, &factorBuffer);
	deviceContext->VSSetConstantBuffers(1, 1, &factorBuffer);

	//Additional
// Send light data to pixel shader
	LightBufferType* lightPtr;
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (LightBufferType*)mappedResource.pData;
	lightPtr->ambient[0] = light->getAmbientColour();
	lightPtr->ambient[1] = light1->getAmbientColour();
	lightPtr->ambient[2] = light2->getAmbientColour();
	lightPtr->ambient[3] = light3->getAmbientColour();
	lightPtr->diffuse[0] = light->getDiffuseColour();
	lightPtr->diffuse[1] = light1->getDiffuseColour();
	lightPtr->diffuse[2] = light2->getDiffuseColour();
	lightPtr->diffuse[3] = light3->getDiffuseColour();
	lightPtr->direction[0] = XMFLOAT4(light->getDirection().x, light->getDirection().y, light->getDirection().z, 0.0f);
	lightPtr->direction[1] = XMFLOAT4(light1->getDirection().x, light1->getDirection().y, light1->getDirection().z, 0.0f);
	lightPtr->direction[2] = XMFLOAT4(light2->getDirection().x, light2->getDirection().y, light2->getDirection().z, 0.0f);
	lightPtr->direction[3] = XMFLOAT4(light3->getDirection().x, light3->getDirection().y, light3->getDirection().z, 0.0f);
	lightPtr->position[0] = XMFLOAT4(light1->getPosition().x, light1->getPosition().y, light1->getPosition().z, 0.0);
	lightPtr->position[1] = XMFLOAT4(light2->getPosition().x, light2->getPosition().y, light2->getPosition().z, 0.0);
	lightPtr->position[2] = XMFLOAT4(light3->getPosition().x, light3->getPosition().y, light3->getPosition().z, 0.0);
	deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

	deviceContext->DSSetShaderResources(0, 1, &texture);
	deviceContext->DSSetSamplers(0, 1, &sampleState);

	deviceContext->PSSetShaderResources(0, 1, &texture2);
	deviceContext->PSSetSamplers(0, 1, &sampleState);

	deviceContext->PSSetShaderResources(1, 1, &shadowmap);
	deviceContext->PSSetSamplers(1, 1, &sampleStateShadow);
}


