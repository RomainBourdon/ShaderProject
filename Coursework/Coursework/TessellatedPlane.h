// Quad Mesh
// Simple square mesh, with texture coordinates and normals.

#ifndef _TESSELLATEDPLANE_H_
#define _TESSELLATEDPLANE_H_

#include "BaseMesh.h"

using namespace DirectX;

class TessellatedPlane : public BaseMesh
{

public:
	TessellatedPlane(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~TessellatedPlane();

protected:
	void initBuffers(ID3D11Device* device);

public:
	void sendData(ID3D11DeviceContext* deviceContext);
};
#endif

