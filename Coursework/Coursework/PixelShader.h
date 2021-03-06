#include "DXF.h"	// include dxframework

#include "BaseShader.h"

using namespace std;
using namespace DirectX;

class PixelShader : public BaseShader
{
public:
	PixelShader(ID3D11Device* device, HWND hwnd);
	~PixelShader();

	struct DepthBufferType
	{
		float nearVal;
		float farVal;
		float offset;
		float range;
	};

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* texture2, ID3D11ShaderResourceView* depthmap, float nearest, float farest, float offset, float range);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateDepth;
	ID3D11Buffer* DepthBuffer;
	
};

