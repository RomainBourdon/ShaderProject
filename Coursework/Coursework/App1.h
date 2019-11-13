// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework
#include "TessellationShader.h"
#include "TessellatedPlane.h"
#include "LightShader.h"

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void gui();

private:
	TessellatedPlane* mesh;
	SphereMesh* firefly;
	TessellationShader* tessshader;
	LightShader* lightshader;
	Light* fireflylight[3];
	Light* worldLight;
	float elapsedtime;
};

#endif