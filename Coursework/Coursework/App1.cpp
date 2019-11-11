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
	mesh = new TessellatedPlane(renderer->getDevice(), renderer->getDeviceContext());
	tessshader = new TessellationShader(renderer->getDevice(), hwnd);

	textureMgr->loadTexture(L"brick", L"res/height.png");
	textureMgr->loadTexture(L"Mountain", L"res/TexturesCom_RockGrassy0019_M.jpg");

	lightshader = new LightShader(renderer->getDevice(), hwnd);

	light = new Light;
	light->setAmbientColour(0.2f, 0.2f, 0.2f, 1.0f);
	light->setDiffuseColour(1.0f, 0.0f, 0.0f, 1.0f);
	light->setDirection(0.0f, -1.0f, 0.0f);
	light->setPosition(45.0f, 10.0f, 50.0f);
	light->setSpecularColour(1.0f, 1.0f, 1.0f, 1.0f);
	light->setSpecularPower(50.0f);

	light1 = new Light;
	light1->setAmbientColour(0.2f, 0.2f, 0.2f, 1.0f);
	light1->setDiffuseColour(0.0f, 1.0f, 0.0f, 1.0f);
	light1->setDirection(0.0f, -1.0f, 0.0f);
	light1->setPosition(55.0f, 10.0f, 50.0f);
	light1->setSpecularColour(1.0f, 1.0f, 1.0f, 1.0f);
	light1->setSpecularPower(50.0f);
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
	tessshader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, elapsedtime, camera->getPosition(), textureMgr->getTexture(L"brick"), textureMgr->getTexture(L"Mountain"), light, light1);
	tessshader->render(renderer->getDeviceContext(), mesh->getIndexCount());

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

