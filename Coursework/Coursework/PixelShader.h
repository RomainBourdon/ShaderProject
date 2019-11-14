
#include "BaseShader.h"

using namespace std;
using namespace DirectX;

class PixelShader : public BaseShader
{
public:
	PixelShader(ID3D11Device* device, HWND hwnd);
	~PixelShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* texture2);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
};

