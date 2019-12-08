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
	//load in the textures 
	textureMgr->loadTexture(L"brick", L"res/height.png");
	textureMgr->loadTexture(L"Mountain", L"res/TexturesCom_RockGrassy0019_M.jpg");

	//initialise the geometry
	mesh = new TessellatedPlane(renderer->getDevice(), renderer->getDeviceContext());
	firefly = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());
	orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);	// Full screen size
	orthoMesh2 = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);	// Full screen size
	
	//shadow map initialisation
	shadowMap = new ShadowMap(renderer->getDevice(), 2048, 2048);

	//shader initialisations
	blurshader = new BlurShader(renderer->getDevice(), hwnd);
	tessshader = new TessellationShader(renderer->getDevice(), hwnd);	
	lightshader = new LightShader(renderer->getDevice(), hwnd);
	pixelshader = new PixelShader(renderer->getDevice(), hwnd);
	depthshader = new DepthShader(renderer->getDevice(), hwnd);
	tessdepthshader = new TessellationDepthShader(renderer->getDevice(), hwnd);

	//render ti texture initialisations
	blurRTT = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	sceneRTT = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	depthRTT = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	//light initialisation
	for (int i = 0; i < 3; i++)
	{
		
		fireflylight[i] = new Light;
		fireflylight[i]->setAmbientColour(0.05f, 0.05f, 0.05f, 1.0f);
		fireflylight[i]->setDiffuseColour(0.678f, 1.0f, 0.184f, 1.0f);
		fireflylight[i]->setDirection(0.0f, -1.0f, 0.0f);
	}

	worldLight = new Light;
	worldLight->setAmbientColour(0.2f, 0.2f, 0.2f, 1.0f);
	worldLight->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	worldLight->setDirection(1.0f, -0.2f, 0.0f);
	worldLight->setPosition(-10.0f, 10.0f, 50.0f);
	worldLight->generateOrthoMatrix((float)100, (float)100, 0.1f, 200.f);

	//initialise light variables to be manipulated by ImGui
	lightpos[0] = -10;
	lightpos[1] = 10;
	lightpos[2] = 50;

	dir[0] = 0.7f;
	dir[1] = -0.3f;
	dir[2] = 0.0f;

	for (int i = 0; i < 3; ++i)
	{
		colour[i] = 1;
	}
	HeightMapMul = 15;
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
	//get delta time
	elapsedtime += timer->getTime();
	
	//calculate the waves for the fireflies to move in a figure of 8
	// 1 sin wave
	sinX = sin((fireflylight[0]->getPosition().y * 0.5) + (elapsedtime));
	// 2 cos waves
	cosY = cos((fireflylight[0]->getPosition().x * 0.5) + (elapsedtime * 0.5));
	
	//set the waves at the fireflies position every frame
	fireflylight[0]->setPosition(45 + sinX, 2 + cosY, 10);
	fireflylight[1]->setPosition(20 + sinX, 2 + cosY, 80); 
	fireflylight[2]->setPosition(90 + sinX, 2 + cosY, 30);

	//allow the world lights position and direction to change at run time
	worldLight->setPosition(lightpos[0], lightpos[1], lightpos[2]);

	worldLight->setDirection(dir[0], dir[1], dir[2]);

	worldLight->setDiffuseColour(colour[0], colour[1], colour[2], 1.0f);
	HeightMapMul = HeightMapMul;
	
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
	//render passes in order
	DepthShadowPass();

	ScenePass();

	BlurPass();

	DepthFieldPass();

	FinalPass();

	return true;
}

void App1::ScenePass()
{	
	//set the render target to be the scene render to texture
	sceneRTT->clearRenderTarget(renderer->getDeviceContext(), 0.941f, 0.268f, 0.137f, 1.0f);
	sceneRTT->setRenderTarget(renderer->getDeviceContext());
	

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	// Send tessellsted plane data, set shader parameters, render with shader
	mesh->sendData(renderer->getDeviceContext());
	tessshader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, elapsedtime, camera->getPosition(), textureMgr->getTexture(L"brick"), 
		textureMgr->getTexture(L"Mountain"), worldLight, fireflylight[0], fireflylight[1], fireflylight[2], shadowMap->getDepthMapSRV(), HeightMapMul);
	tessshader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	//set the sphere meshes world matrix to be at the spotlights lights position
	worldMatrix = XMMatrixTranslation(fireflylight[0]->getPosition().x, fireflylight[0]->getPosition().y, fireflylight[0]->getPosition().z);

	firefly->sendData(renderer->getDeviceContext());
	lightshader->setShaderParameters(renderer->getDeviceContext(), (XMMatrixScaling(0.05, 0.05, 0.05) * worldMatrix), viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"), fireflylight[0], fireflylight[1], fireflylight[2], elapsedtime);
	lightshader->render(renderer->getDeviceContext(), firefly->getIndexCount());

	worldMatrix = XMMatrixTranslation(fireflylight[1]->getPosition().x, fireflylight[1]->getPosition().y, fireflylight[1]->getPosition().z);
	
	lightshader->setShaderParameters(renderer->getDeviceContext(), (XMMatrixScaling(0.05, 0.05, 0.05) * worldMatrix), viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"), fireflylight[0], fireflylight[1], fireflylight[2], elapsedtime);
	lightshader->render(renderer->getDeviceContext(), firefly->getIndexCount());

	worldMatrix = XMMatrixTranslation(fireflylight[2]->getPosition().x, fireflylight[2]->getPosition().y, fireflylight[2]->getPosition().z);

	lightshader->setShaderParameters(renderer->getDeviceContext(), (XMMatrixScaling(0.05, 0.05, 0.05) * worldMatrix), viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"), fireflylight[0], fireflylight[1], fireflylight[2], elapsedtime);
	lightshader->render(renderer->getDeviceContext(), firefly->getIndexCount());

	//reset render target
	renderer->setBackBufferRenderTarget();
}

void App1::BlurPass()
{
	//set render target to be the blurred render to texture
	blurRTT->setRenderTarget(renderer->getDeviceContext());
	blurRTT->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 1.0f, 0.0f, 1.0f);

	XMMATRIX worldMatrix, viewMatrix, orthoMatrix;

	//get the scene render to texture's width and height
	float texsizeX = (float)sceneRTT->getTextureWidth();
	float texsizeY = (float)sceneRTT->getTextureHeight();

	// Get the world and view from the camera and and get the ortho view matrix from the render target.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = blurRTT->getOrthoMatrix();

	//set z buufer off
	renderer->setZBuffer(false);
	orthoMesh->sendData(renderer->getDeviceContext());
	blurshader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, orthoMatrix, sceneRTT->getShaderResourceView(), texsizeX, texsizeY);
	blurshader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);
	//reset render target
	renderer->setBackBufferRenderTarget();
}

void App1::DepthFieldPass()
{
	//set render target to be the depth render to texture
	depthRTT->setRenderTarget(renderer->getDeviceContext());
	depthRTT->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 0.0f, 0.0f, 1.0f);

	//Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX cameraViewMatrix = camera->getViewMatrix();
	XMMATRIX ProjectionMatrix = renderer->getProjectionMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	//send plane data to the shaders and render with that shader
	mesh->sendData(renderer->getDeviceContext());
	tessdepthshader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, cameraViewMatrix, ProjectionMatrix, elapsedtime, camera->getPosition(), worldLight, HeightMapMul);
	tessdepthshader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	worldMatrix = XMMatrixTranslation(fireflylight[0]->getPosition().x, fireflylight[0]->getPosition().y, fireflylight[0]->getPosition().z);

	firefly->sendData(renderer->getDeviceContext());
	depthshader->setShaderParameters(renderer->getDeviceContext(), (XMMatrixScaling(0.05, 0.05, 0.05) * worldMatrix), cameraViewMatrix, ProjectionMatrix);
	depthshader->render(renderer->getDeviceContext(), firefly->getIndexCount());

	worldMatrix = XMMatrixTranslation(fireflylight[1]->getPosition().x, fireflylight[1]->getPosition().y, fireflylight[1]->getPosition().z);

	depthshader->setShaderParameters(renderer->getDeviceContext(), (XMMatrixScaling(0.05, 0.05, 0.05) * worldMatrix), cameraViewMatrix, ProjectionMatrix);
	depthshader->render(renderer->getDeviceContext(), firefly->getIndexCount());

	worldMatrix = XMMatrixTranslation(fireflylight[2]->getPosition().x, fireflylight[2]->getPosition().y, fireflylight[2]->getPosition().z);

	depthshader->setShaderParameters(renderer->getDeviceContext(), (XMMatrixScaling(0.05, 0.05, 0.05) * worldMatrix), cameraViewMatrix, ProjectionMatrix);
	depthshader->render(renderer->getDeviceContext(), firefly->getIndexCount());

	renderer->setBackBufferRenderTarget();
}

void App1::DepthShadowPass()
{
	//set the shadow map to be the render target
	shadowMap->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	//Get the world, view, projection, and ortho matrices from the world light.
	worldLight->generateViewMatrix();
	XMMATRIX lightViewMatrix = worldLight->getViewMatrix();
	XMMATRIX lightProjectionMatrix = worldLight->getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	
	//send plane data to the shaders and render with that shader
	mesh->sendData(renderer->getDeviceContext());
	tessdepthshader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, elapsedtime, camera->getPosition(), worldLight, HeightMapMul);
	tessdepthshader->render(renderer->getDeviceContext(), mesh->getIndexCount());	

	worldMatrix = XMMatrixTranslation(fireflylight[0]->getPosition().x, fireflylight[0]->getPosition().y, fireflylight[0]->getPosition().z);

	firefly->sendData(renderer->getDeviceContext());
	depthshader->setShaderParameters(renderer->getDeviceContext(), (XMMatrixScaling(0.05, 0.05, 0.05) * worldMatrix), lightViewMatrix, lightProjectionMatrix);
	depthshader->render(renderer->getDeviceContext(), firefly->getIndexCount());

	worldMatrix = XMMatrixTranslation(fireflylight[1]->getPosition().x, fireflylight[1]->getPosition().y, fireflylight[1]->getPosition().z);

	depthshader->setShaderParameters(renderer->getDeviceContext(), (XMMatrixScaling(0.05, 0.05, 0.05) * worldMatrix), lightViewMatrix, lightProjectionMatrix);
	depthshader->render(renderer->getDeviceContext(), firefly->getIndexCount());

	worldMatrix = XMMatrixTranslation(fireflylight[2]->getPosition().x, fireflylight[2]->getPosition().y, fireflylight[2]->getPosition().z);

	depthshader->setShaderParameters(renderer->getDeviceContext(), (XMMatrixScaling(0.05, 0.05, 0.05) * worldMatrix), lightViewMatrix, lightProjectionMatrix);
	depthshader->render(renderer->getDeviceContext(), firefly->getIndexCount());

	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
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
	pixelshader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, sceneRTT->getShaderResourceView(), blurRTT->getShaderResourceView(), depthRTT->getShaderResourceView(), SCREEN_NEAR, SCREEN_DEPTH, 0.0, 1.0);
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
	ImGui::SliderFloat("world light x", &lightpos[0], -100, 100);
	ImGui::SliderFloat("world light y", &lightpos[1], -100, 100);
	ImGui::SliderFloat("world light z", &lightpos[2], -100, 100);
	ImGui::SliderFloat("world light dirx", &dir[0], -1, 1);
	ImGui::SliderFloat("world light diry", &dir[1], -1, 1);
	ImGui::SliderFloat("world light dirz", &dir[2], -1, 1);
	ImGui::SliderFloat("height map multiplier", &HeightMapMul, 0, 30);
	ImGui::ColorPicker3("world light colour", colour);

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

