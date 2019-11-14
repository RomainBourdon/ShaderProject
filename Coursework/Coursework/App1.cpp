// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{

}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Initalise scene variables.
	
	textureMgr->loadTexture(L"brick", L"res/height.png");
	textureMgr->loadTexture(L"Mountain", L"res/TexturesCom_RockGrassy0019_M.jpg");

	mesh = new TessellatedPlane(renderer->getDevice(), renderer->getDeviceContext());
	orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);	// Full screen size
	orthoMesh2 = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);	// Full screen size

	firefly = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());

	blurshader = new BlurShader(renderer->getDevice(), hwnd);
	tessshader = new TessellationShader(renderer->getDevice(), hwnd);	
	lightshader = new LightShader(renderer->getDevice(), hwnd);
	pixelshader = new PixelShader(renderer->getDevice(), hwnd);

	blurRTT = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	sceneRTT = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	depthRTT = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	for (int i = 0; i < 3; i++)
	{
		
		fireflylight[i] = new Light;
		fireflylight[i]->setAmbientColour(0.05f, 0.05f, 0.05f, 1.0f);
		fireflylight[i]->setDiffuseColour(0.678f, 1.0f, 0.184f, 1.0f);
		fireflylight[i]->setDirection(0.0f, -1.0f, 0.0f);
	}

	worldLight = new Light;
	worldLight->setAmbientColour(0.05f, 0.05f, 0.05f, 1.0f);
	worldLight->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	worldLight->setDirection(-0.7f, -0.7f, 0.0f);
	worldLight->setPosition(-100.0f, 100.0f, 100.0f);
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	
}


bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}
	elapsedtime += timer->getTime();
	
	sinX = sin((fireflylight[0]->getPosition().y * 0.5) + (elapsedtime));
	cosY = cos((fireflylight[0]->getPosition().x * 0.5) + (elapsedtime * 0.5));
	
	
	fireflylight[0]->setPosition(45 + sinX, 2 + cosY, 10);
	fireflylight[1]->setPosition(20 + sinX, 2 + cosY, 80); 
	fireflylight[2]->setPosition(90 + sinX, 2 + cosY, 30);
	
	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{
	ScenePass();

	BlurPass();

	//DepthPass();

	FinalPass();

	return true;
}

void App1::ScenePass()
{
	sceneRTT->setRenderTarget(renderer->getDeviceContext());
	sceneRTT->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 1.0f, 1.0f);

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	// Send geometry data, set shader parameters, render object with shader
	mesh->sendData(renderer->getDeviceContext());
	tessshader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, elapsedtime, camera->getPosition(), textureMgr->getTexture(L"brick"), textureMgr->getTexture(L"Mountain"), worldLight, fireflylight[0], fireflylight[1], fireflylight[2]);
	tessshader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	worldMatrix = XMMatrixTranslation(fireflylight[0]->getPosition().x, fireflylight[0]->getPosition().y, fireflylight[0]->getPosition().z);

	firefly->sendData(renderer->getDeviceContext());
	lightshader->setShaderParameters(renderer->getDeviceContext(), (XMMatrixScaling(0.05, 0.05, 0.05) * worldMatrix), viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"), fireflylight[0], fireflylight[1], fireflylight[2], elapsedtime);
	lightshader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	worldMatrix = XMMatrixTranslation(fireflylight[1]->getPosition().x, fireflylight[1]->getPosition().y, fireflylight[1]->getPosition().z);
	
	lightshader->setShaderParameters(renderer->getDeviceContext(), (XMMatrixScaling(0.05, 0.05, 0.05) * worldMatrix), viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"), fireflylight[0], fireflylight[1], fireflylight[2], elapsedtime);
	lightshader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	worldMatrix = XMMatrixTranslation(fireflylight[2]->getPosition().x, fireflylight[2]->getPosition().y, fireflylight[2]->getPosition().z);

	lightshader->setShaderParameters(renderer->getDeviceContext(), (XMMatrixScaling(0.05, 0.05, 0.05) * worldMatrix), viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"), fireflylight[0], fireflylight[1], fireflylight[2], elapsedtime);
	lightshader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	renderer->setBackBufferRenderTarget();

}

void App1::BlurPass()
{
	blurRTT->setRenderTarget(renderer->getDeviceContext());
	blurRTT->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 1.0f, 1.0f);


	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;
	float texsizeX = (float)sceneRTT->getTextureWidth();
	float texsizeY = (float)sceneRTT->getTextureHeight();

	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = blurRTT->getOrthoMatrix();

	renderer->setZBuffer(false);
	orthoMesh->sendData(renderer->getDeviceContext());
	blurshader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix, sceneRTT->getShaderResourceView(), texsizeX, texsizeY);
	blurshader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);
	renderer->setBackBufferRenderTarget();
}

void App1::DepthPass()
{

}

void App1::FinalPass()
{
	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	XMMATRIX worldMatrix;

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();

	renderer->setZBuffer(false);

	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering
	
	orthoMesh2->sendData(renderer->getDeviceContext());
	pixelshader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, sceneRTT->getShaderResourceView(), blurRTT->getShaderResourceView());
	pixelshader->render(renderer->getDeviceContext(), orthoMesh2->getIndexCount());

	renderer->setZBuffer(true);

	// Render GUI
	gui();

	// Swap the buffers
	renderer->endScene();
}



void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

