// Light shader.h
// Basic single light shader setup
#pragma once

#include "DXF.h"


using namespace std;
using namespace DirectX;


class TessellationShader : public BaseShader
{
private:
	struct const_edgesBufferType
	{
		XMFLOAT3 camera;
		float time;
	};
	struct LightBufferType
	{
		XMFLOAT4 ambient[2];
		XMFLOAT4 diffuse[2];
		XMFLOAT4 direction[2];
		XMFLOAT4 specColour;
		XMFLOAT4 position[2];
		XMFLOAT4 power;
	};

public:

	TessellationShader(ID3D11Device* device, HWND hwnd);
	~TessellationShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, float time, XMFLOAT3 camera, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* texture2, Light* light, Light* light1);

private:
	void initShader(const wchar_t* vsFilename, const wchar_t* psFilename);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* factorBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11SamplerState* sampleState;

};


