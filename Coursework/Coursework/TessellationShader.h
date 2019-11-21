// Light shader.h
// Basic single light shader setup
#pragma once

#include "DXF.h"


using namespace std;
using namespace DirectX;


class TessellationShader : public BaseShader
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView;
		XMMATRIX lightProjection;
	};

	struct const_edgesBufferType
	{
		XMFLOAT3 camera;
		float time;
	};
	struct LightBufferType
	{
		XMFLOAT4 ambient[4];
		XMFLOAT4 diffuse[4];
		XMFLOAT4 direction[4];
		XMFLOAT4 position[3];
	};

public:

	TessellationShader(ID3D11Device* device, HWND hwnd);
	~TessellationShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, float time, XMFLOAT3 camera, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* texture2, Light* light, Light* light1, Light* light2, Light* light3, ID3D11ShaderResourceView* shadowmap);

private:
	void initShader(const wchar_t* vsFilename, const wchar_t* psFilename);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* factorBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;
};


