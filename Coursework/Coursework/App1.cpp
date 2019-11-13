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
	tessshader = new TessellationShader(renderer->getDevice(), hwnd);
	lightshader = new LightShader(renderer->getDevice(), hwnd);

	firefly = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());
	for (int i = 0; i < 3; i++)
	{
		
		fireflylight[i] = new Light;
		fireflylight[i]->setAmbientColour(0.05f, 0.05f, 0.05f, 1.0f);
		fireflylight[i]->setDiffuseColour(0.678f, 1.0f, 0.184f, 1.0f);
		fireflylight[i]->setDirection(0.0f, -1.0f, 0.0f);
	}

	fireflylight[0]->setPosition(45.0f, 2.0f, 10.0f);
	fireflylight[1]->setPosition(47.0f, 2.0f, 13.0f);
	fireflylight[2]->setPosition(49.0f, 2.0f, 10.0f);

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
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

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
	firefly->sendData(renderer->getDeviceContext());
	lightshader->setShaderParameters(renderer->getDeviceContext(), (XMMatrixScaling(0.05, 0.05, 0.05) * worldMatrix), viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"), fireflylight[0], fireflylight[1], fireflylight[2], elapsedtime);
	lightshader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	worldMatrix = XMMatrixTranslation(fireflylight[2]->getPosition().x, fireflylight[2]->getPosition().y, fireflylight[2]->getPosition().z);
	firefly->sendData(renderer->getDeviceContext());
	lightshader->setShaderParameters(renderer->getDeviceContext(), (XMMatrixScaling(0.05, 0.05, 0.05)* worldMatrix), viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"), fireflylight[0], fireflylight[1], fireflylight[2], elapsedtime);
	lightshader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	// Render GUI
	gui();

	// Swap the buffers
	renderer->endScene();

	return true;
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

