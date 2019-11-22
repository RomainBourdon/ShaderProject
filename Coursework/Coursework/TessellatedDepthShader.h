#pragma once

#include "DXF.h"


using namespace std;
using namespace DirectX;


class TessellationDepthShader : public BaseShader
{
private:
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
	};

	struct const_edgesBufferType
	{
		XMFLOAT3 camera;
		float time;
	};

public:

	TessellationDepthShader(ID3D11Device* device, HWND hwnd);
	~TessellationDepthShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, float time, XMFLOAT3 camera);

private:
	void initShader(const wchar_t* vsFilename, const wchar_t* psFilename);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* factorBuffer;
	ID3D11SamplerState* sampleState;
};

