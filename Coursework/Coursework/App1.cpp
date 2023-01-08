#include "App1.h"

App1::App1()
{

}
void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Create Mesh object and shader object
	grassplane = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext(), 800);
	WaterPlane = new TessellationPlane(renderer->getDevice(), renderer->getDeviceContext(), 100);
	Groundplane = new TessellationPlane(renderer->getDevice(), renderer->getDeviceContext(), 100);

	//ground shader
	tesselation = new TessellationShader(renderer->getDevice(), hwnd);
	//grass shader
	geometryShader = new GeometryShader(renderer->getDevice(), hwnd);
	//Water shader
	waterTessellation = new WaterTessellation(renderer->getDevice(), hwnd);

	//Bloom shaders
	verticalBlurShader = new VerticalBlurShader(renderer->getDevice(), hwnd);
	horizontalBlurShader = new HorizontalBlurShader(renderer->getDevice(), hwnd);
	colourRemoveShader = new ColorRemoveShader(renderer->getDevice(), hwnd);
	bloomShader = new BasicShader(renderer->getDevice(), hwnd);

	orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);
	//base pass
	baseTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	//decreased colour pass
	decresedColourTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	//horizontal blur
	horizontalBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	//vertical blur
	verticalBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);

	//depth shaders
	depthShader = new DepthShader(renderer->getDevice(), hwnd, 0);
	waterDepthShader = new DepthShader(renderer->getDevice(), hwnd, 1);
	grassDepthShader = new DepthShader(renderer->getDevice(), hwnd, 2);

	//Load textures
	textureMgr->loadTexture(L"heightMap", L"res/heightMap.png");
	textureMgr->loadTexture(L"water", L"res/water.png");
	textureMgr->loadTexture(L"color", L"res/texture.png");
	textureMgr->loadTexture(L"grass", L"res/grass.png");

	//Shadow map values
	int shadowmapWidth = 1024;
	int shadowmapHeight = 1024;

	int sceneWidth = 100;
	int sceneHeight = 100;

	//init shadowmaps
	for (int i = 0; i < 3; i++)
		shadowMap[i] = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);

	//first light.
	light[0] = new Light();
	light[0]->setPosition(0, 20, 0);
	light[0]->setDiffuseColour(1, 1, 1, 1);
	light[0]->setAmbientColour(0.2f, 0.2f, 0.2f, 1.0f);
	light[0]->setDirection(0, -1, 0);
	light[0]->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1f, 100.f);

	//second light.
	light[1] = new Light();
	light[1]->setPosition(50, 20, 50);
	light[1]->setDiffuseColour(1, 1, 1, 1);
	light[1]->setAmbientColour(0.2f, 0.2f, 0.2f, 1.0f);
	light[1]->setDirection(0, -1, 0);
	light[1]->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1f, 100.f);


	//third light.
	light[2] = new Light();
	light[2]->setPosition(25, 100, 25);
	light[2]->setDiffuseColour(1, 1, 1, 1);
	light[2]->setAmbientColour(0.2f, 0.2f, 0.2f, 1.0f);
	light[2]->setDirection(0, -1, 0);
	light[2]->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1f, 100.f);

	//set up all lights
	for (int i = 0; i < 4; i++)
	{
		//diffuse values
		diff[i] = 1.0f;
		diffTwo[i] = 1.0f;
		diffThree[i] = 1.0f;

		if (i < 3)
		{
			pos[i] = 25.0f;
			posTwo[i] = 50.0f;
			posThree[i] = 25.0f;
		
			if (i == 1)
			{
				pos[i] = 20.0f;
				posTwo[i] = 20.0f;
				posThree[i] = 20.0f;
			}
		}
	}

	//set directional values
	dir[0] = 0.f;
	dir[1] = -1.f;
	dir[2] = 0.f;

	//set camera starting position
	camera->setPosition(25, 25, 25);
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release objects.
	TessellationShader* tesselation;
	WaterTessellation* waterTessellation;
	VerticalBlurShader* verticalBlurShader;
	HorizontalBlurShader* horizontalBlurShader;
	ColorRemoveShader* colourRemoveShader;
	BasicShader* bloomShader;

	//Meshes
	PlaneMesh* grassplane;
	TessellationPlane* WaterPlane;
	TessellationPlane* Groundplane;

	OrthoMesh* orthoMesh;
	RenderTexture* baseTexture;
	RenderTexture* horizontalBlurTexture;
	RenderTexture* verticalBlurTexture;
	RenderTexture* decresedColourTexture;

	ID3D11ShaderResourceView* depthMaps[3];
	ShadowMap* shadowMap[3];

	DepthShader* depthShader;
	DepthShader* grassDepthShader;
	DepthShader* waterDepthShader;

	Light* light[3];

}

bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
		return false;

	result = render();
	if (!result)
		return false;
	time += timer->getTime();

	//light 1
	light[0]->setPosition(pos[0], pos[1], pos[2]);
	light[0]->setDiffuseColour(diff[0], diff[1], diff[2], diff[3]);
	//light 2
	light[1]->setPosition(posTwo[0], posTwo[1], posTwo[2]);
	light[1]->setDiffuseColour(diffTwo[0], diffTwo[1], diffTwo[2], diffTwo[3]);
	//light 3
	light[2]->setPosition(posThree[0], posThree[1], posThree[2]);
	light[2]->setDiffuseColour(diffThree[0], diffThree[1], diffThree[2], diffThree[3]);
	//check light
	checkLight();
	return true;
}

bool App1::render()
{
	//depth pass for lights
	depthPassOne();
	depthPassTwo();
	depthPassThree();

	//store depth maps
	for (int i = 0; i < 3; i++)
		depthMaps[i] = shadowMap[i]->getDepthMapSRV();

	//toggle bloom on and off
	if (bloomToggle)
		bloom();
	else
	{
		//background colour
		renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

		//shadows and no bloom
		renderWithLights();

		gui();

		//swap buffers
		renderer->endScene();
	}
	return true;
}

void App1::bloom()
{
	//Clean pass
	firstPass();
	//colour remove pass
	secondPass();
	//Horizontal blur pass
	thirdPass();
	//Vertical blur pass
	fourthPass();
	//Combine for final pass
	finalPass();
}

void App1::depthPassOne()
{
	// Set the render target to be the render to texture.
	shadowMap[0]->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	light[0]->generateViewMatrix();
	XMMATRIX lightViewMatrix = light[0]->getViewMatrix();
	XMMATRIX lightProjectionMatrix = light[0]->getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();


	//store grass values in a float 4
	XMFLOAT4 slide_cam = XMFLOAT4(slide, camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
	XMFLOAT3 tsl = XMFLOAT3(time, speed, limit);
	//Render grass in depth shader
	grassplane->sendData(renderer->getDeviceContext());
	grassDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"heightMap"), slide_cam, tsl, slide, lHeight, uHeight);
	grassDepthShader->render(renderer->getDeviceContext(), grassplane->getIndexCount());

	//water in depth shader
	WaterPlane->sendData(renderer->getDeviceContext());
	waterDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, time, amper, length, speedW, distance, camera->getPosition());
	waterDepthShader->render(renderer->getDeviceContext(), WaterPlane->getIndexCount());

	//ground in depth shader
	Groundplane->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"heightMap"));
	depthShader->render(renderer->getDeviceContext(), Groundplane->getIndexCount());


	//swap buffers
	renderer->setBackBufferRenderTarget();
	//reset viewport
	renderer->resetViewport();
}

void App1::depthPassTwo()
{
	shadowMap[1]->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	light[1]->generateViewMatrix();
	XMMATRIX lightViewMatrix = light[1]->getViewMatrix();
	XMMATRIX lightProjectionMatrix = light[1]->getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	//store grass values in a float 4
	XMFLOAT4 slide_cam = XMFLOAT4(slide, camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
	XMFLOAT3 tsl = XMFLOAT3(time, speed, limit);
	//grass in depth shader
	grassplane->sendData(renderer->getDeviceContext());
	grassDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"heightMap"), slide_cam, tsl, slide, lHeight, uHeight);
	grassDepthShader->render(renderer->getDeviceContext(), grassplane->getIndexCount());

	//water in depth shader
	WaterPlane->sendData(renderer->getDeviceContext());
	waterDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, time, amper, length, speedW, distance, camera->getPosition());
	waterDepthShader->render(renderer->getDeviceContext(), WaterPlane->getIndexCount());

	//Ground in depth shader
	Groundplane->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"heightMap"));
	depthShader->render(renderer->getDeviceContext(), Groundplane->getIndexCount());


	//swap buffers
	renderer->setBackBufferRenderTarget();
	//reset viewport
	renderer->resetViewport();
}

void App1::depthPassThree()
{
	shadowMap[2]->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	light[2]->generateViewMatrix();
	XMMATRIX lightViewMatrix = light[2]->getViewMatrix();
	XMMATRIX lightProjectionMatrix = light[2]->getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	//store grass values in a float 4
	XMFLOAT4 slide_cam = XMFLOAT4(slide, camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
	XMFLOAT3 tsl = XMFLOAT3(time, speed, limit);
	//grass in depth shader
	grassplane->sendData(renderer->getDeviceContext());
	grassDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"heightMap"), slide_cam, tsl, slide, lHeight, uHeight);
	grassDepthShader->render(renderer->getDeviceContext(), grassplane->getIndexCount());

	//water in depth shader
	WaterPlane->sendData(renderer->getDeviceContext());
	waterDepthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, time, amper, length, speedW, distance, camera->getPosition());
	waterDepthShader->render(renderer->getDeviceContext(), WaterPlane->getIndexCount());

	//ground in depth shader
	Groundplane->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"heightMap"));
	depthShader->render(renderer->getDeviceContext(), Groundplane->getIndexCount());

	//swap buffers
	renderer->setBackBufferRenderTarget();
	//reset viewport
	renderer->resetViewport();
}

void App1::renderWithLights()
{
	camera->update();

	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	//store grass values in float 4
	XMFLOAT4 cam_time = XMFLOAT4(camera->getPosition().x, camera->getPosition().y, camera->getPosition().z, time);
	XMFLOAT4 sllu = XMFLOAT4(speed, limit, lHeight, uHeight);

	//grass
	if (grassEnabled)
	{
		grassplane->sendData(renderer->getDeviceContext());
		geometryShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"grass"), textureMgr->getTexture(L"heightMap"), cam_time, sllu);
		geometryShader->render(renderer->getDeviceContext(), grassplane->getIndexCount());
	}

	//ground
	Groundplane->sendData(renderer->getDeviceContext());
	tesselation->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"heightMap"), textureMgr->getTexture(L"color"), camera->getPosition(), light, depthMaps);
	tesselation->render(renderer->getDeviceContext(), Groundplane->getIndexCount());

	//water
	if (waterEnabled)
	{
		XMFLOAT4 DALS = XMFLOAT4(time, amper, length, speedW);
		renderer->setAlphaBlending(true);
		WaterPlane->sendData(renderer->getDeviceContext());
		waterTessellation->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"water"), DALS, distance, camera->getPosition(), depthMaps, light);
		waterTessellation->render(renderer->getDeviceContext(), WaterPlane->getIndexCount());
		renderer->setAlphaBlending(false);
	}
}


void App1::firstPass()
{
	baseTexture->setRenderTarget(renderer->getDeviceContext());
	baseTexture->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);

	//with lights
	renderWithLights();

	//swap buffers
	renderer->setBackBufferRenderTarget();
}

void App1::secondPass()
{
	decresedColourTexture->setRenderTarget(renderer->getDeviceContext());
	decresedColourTexture->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);

	//set up matrices
	camera->update();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getOrthoViewMatrix();
	XMMATRIX projectionMatrix = decresedColourTexture->getOrthoMatrix();

	//render texture and reduce colour
	orthoMesh->sendData(renderer->getDeviceContext());

	colourRemoveShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, baseTexture->getShaderResourceView(), remove, threshold);
	colourRemoveShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	//swap buffers
	renderer->setBackBufferRenderTarget();
}

void App1::thirdPass()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	XMFLOAT2 screenSize((float)horizontalBlurTexture->getTextureWidth(), (float)horizontalBlurTexture->getTextureHeight());
	XMFLOAT2 screen = XMFLOAT2(screenSize.x, screenSize.y);

	horizontalBlurTexture->setRenderTarget(renderer->getDeviceContext());
	horizontalBlurTexture->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = horizontalBlurTexture->getOrthoMatrix();

	//horizontal Blur
	renderer->setZBuffer(false);
	orthoMesh->sendData(renderer->getDeviceContext());
	horizontalBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, decresedColourTexture->getShaderResourceView(), screenSize.x);
	horizontalBlurShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	//swap buffers
	renderer->setBackBufferRenderTarget();
}

void App1::fourthPass()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	XMFLOAT2 screenSize((float)verticalBlurTexture->getTextureHeight(), (float)verticalBlurTexture->getTextureWidth());
	verticalBlurTexture->setRenderTarget(renderer->getDeviceContext());
	verticalBlurTexture->clearRenderTarget(renderer->getDeviceContext(), 0.39f, 0.58f, 0.92f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	//render to texture
	orthoMatrix = verticalBlurTexture->getOrthoMatrix();

	//vertical Blur
	renderer->setZBuffer(false);
	orthoMesh->sendData(renderer->getDeviceContext());
	verticalBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, horizontalBlurTexture->getShaderResourceView(), screenSize.y);
	verticalBlurShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	//swap buffers
	renderer->setBackBufferRenderTarget();
}

void App1::finalPass()
{
	//clear scene
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	renderer->setZBuffer(false);
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();
	XMMATRIX orthoMatrix = verticalBlurTexture->getOrthoMatrix();

	//bloom render to texture
	orthoMesh->sendData(renderer->getDeviceContext());
	bloomShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, baseTexture->getShaderResourceView(), verticalBlurTexture->getShaderResourceView());
	bloomShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	renderer->setZBuffer(true);

	gui();

	//swap buffers
	renderer->endScene();
}

void App1::gui()
{
	//turn off unneccessary shaders
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	//set up UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);
	ImGui::Checkbox("Bloom", &bloomControls);
	ImGui::Checkbox("Water", &waterControls);
	ImGui::Checkbox("Grass", &grassControls);

	//grass controls
	if (grassControls)
	{
		ImGui::Begin("Grass");
		ImGui::Checkbox("Toggle Grass", &grassEnabled);
		ImGui::InputFloat("Slide", &slide);
		ImGui::SliderFloat("Speed", &speed, 0, 10);
		ImGui::SliderFloat("Limit", &limit, 4, 10);
		ImGui::SliderFloat("Lower Height", &lHeight, 1, 10);
		ImGui::SliderFloat("Upper Height", &uHeight, 1, 10);
		ImGui::End();
	}
	//water controls
	if (waterControls)
	{
		ImGui::Begin("Water");
		ImGui::Checkbox("Toggle Water", &waterEnabled);
		ImGui::SliderFloat("Speed", &speedW, 0, 5);
		ImGui::SliderFloat("Length", &length, 0, 5);
		ImGui::SliderFloat("Amp", &amper, 0, 5);
		ImGui::InputFloat("Distance", &distance);
		ImGui::End();
	}
	//bloom controls
	if (bloomControls)
	{
		ImGui::Begin("Bloom");
		ImGui::Checkbox("Bloom", &bloomToggle);
		ImGui::SliderFloat("Threshold", &threshold, 0, 1);
		ImGui::SliderFloat("Remove", &remove, 0, 1);
		ImGui::End();
	}

	//light 1
	ImGui::Begin("Light");
	ImGui::Text("Light One");
	ImGui::SliderFloat3("Position1", pos, 0, 100);
	ImGui::SliderFloat4("Diffuse1", diff, 0, 1);
	ImGui::Checkbox("Depth Map 1:", &toggleDepthOne);

	//light 2
	ImGui::Text("Light Two");
	ImGui::SliderFloat3("Position2", posTwo, 0, 100);
	ImGui::SliderFloat4("Diffuse2", diffTwo, 0, 1);
	ImGui::Checkbox("Depth Map 2", &toggleDepthTwo);

	//light 3
	ImGui::Text("Light Three");
	ImGui::SliderFloat3("Position2", posThree, 0, 100);
	ImGui::SliderFloat4("Diffuse2", diffThree, 0, 1);
	ImGui::Checkbox("Depth Map 3", &toggleDepthThree);
	ImGui::End();

	//depth pass 1
	if (toggleDepthOne)
	{
		ImGui::Begin("Light 1");
		{
			ImGui::Image((void*)shadowMap[0]->getDepthMapSRV(), ImVec2(ImGui::GetWindowSize()));
		}
		ImGui::End();
	}

	//depth pass 2
	if (toggleDepthTwo)
	{
		ImGui::Begin("Light 2", &toggleDepthTwo);
		{
			ImGui::Image((void*)shadowMap[1]->getDepthMapSRV(), ImVec2(ImGui::GetWindowSize()));
		}
		ImGui::End();
	}

	//depth pass 3
	if (toggleDepthThree)
	{
		ImGui::Begin("Light 3", &toggleDepthThree);
		{
			ImGui::Image((void*)shadowMap[2]->getDepthMapSRV(), ImVec2(ImGui::GetWindowSize()));
		}
		ImGui::End();
	}
	//draw ui
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

//checks if the light has changed this frame
void App1::checkLight()
{
	int sceneWidth = 100;
	int sceneHeight = 100;
	bool isChanged = false;

	if (dir[0] != dDir[0] || dir[1] != dDir[1] || dir[2] != dDir[2])
	{
		isChanged = true;
	}

	if (isChanged)
	{
		delete(light[2]);
		light[2] = nullptr;

		light[2] = new Light();
		light[2]->setAmbientColour(0.3f, 0.3f, 0.3f, 0.3f);
		light[2]->setDiffuseColour(diffThree[0], diffThree[1], diffThree[2], diffThree[3]);
		light[2]->setDirection(dir[0], dir[1], dir[2]);
		light[2]->setPosition(posThree[0], posThree[1], posThree[2]);
		light[2]->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1f, 100.f);

		for (int i = 0; i < 3; i++)
		{
			dDir[i] = dir[i];
		}

		isChanged = false;
	}
}
