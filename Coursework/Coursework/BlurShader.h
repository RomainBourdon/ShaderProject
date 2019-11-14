#pragma once

#include "DXF.h"

using namespace std;

using namespace DirectX;

class BlurShader : public BaseShader
{
private:
	struct screenBufferType
	{
		float screenWidth;
		float screenHeight;
		XMFLOAT2 padding;
	};

public:
	BlurShader(ID3D11Device* device, HWND hwnd);
	~BlurShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, float scrnwdth, float scrnhgt);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* screenBuffer;
};
