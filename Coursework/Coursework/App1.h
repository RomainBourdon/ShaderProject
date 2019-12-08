// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework
#include "TessellationShader.h"
#include "TessellatedPlane.h"
#include "LightShader.h"
#include "BlurShader.h"
#include "PixelShader.h"
#include "DepthShader.h"
#include "TessellatedDepthShader.h"
 
class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();
	void ScenePass();
	void BlurPass();
	void DepthFieldPass();
	void DepthShadowPass();
	void FinalPass();

protected:
	bool render();
	void gui();

private:
	OrthoMesh* orthoMesh2;
	TessellatedPlane* mesh;
	SphereMesh* firefly;
	OrthoMesh* orthoMesh;
	

	TessellationShader* tessshader;
	LightShader* lightshader;
	BlurShader* blurshader;
	PixelShader* pixelshader;
	TessellationDepthShader* tessdepthshader;
	DepthShader* depthshader;

	ShadowMap* shadowMap;
	
	Light* fireflylight[3];
	Light* worldLight;

	RenderTexture* blurRTT;
	RenderTexture* sceneRTT;
	RenderTexture* depthRTT;
	
	float elapsedtime;
	float cosY;
	float sinX;
	float lightpos[3];
	float dir[3];
	float colour[3];
	float HeightMapMul;
};
#endif