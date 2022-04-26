#include <DDSTextureLoader.h>
#include "Game.h"
#include "Vertex.h"
#include "Input.h"
#include "Lights.h"
#include "WICTextureLoader.h"

// Needed for a helper function to read compiled shader files from the hard drive
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true),			   // Show extra stats (fps) in title bar?
	vsync(false)
{
#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	delete asteroidMaterial;
	delete metalHatchMaterial;
	delete scratchedMaterial;
	delete copperMaterial;
	delete transparentMaterial;
	delete sphereMesh;
	delete cubeMesh;
	delete helixMesh;
	delete skyBox;
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	CreateBasicGeometry();
	SetLights();
	camera = std::make_shared<Camera>(Transform(0, 0, -10, 0, 0, 0, 1, 1, 1), (float)this->width / this->height);
	ambientColor = XMFLOAT3(0.15f, 0.15f, 0.25f);
	
	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D11_BLEND_DESC bd = {};
	bd.AlphaToCoverageEnable = false;
	bd.IndependentBlendEnable = false;
	bd.RenderTarget[0].BlendEnable = true;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&bd, &transparencyBlendState);
	context->OMSetBlendState(transparencyBlendState, NULL, 0xffffffff);
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = std::make_shared<SimpleVertexShader>(device, context, GetFullPathTo_Wide(L"VertexShader.cso").c_str());    
	skyBoxVertexShader = std::make_shared<SimpleVertexShader>(device, context, GetFullPathTo_Wide(L"SkyBoxVertexShader.cso").c_str());
	skyBoxPixelShader = std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"SkyBoxPixelShader.cso").c_str());
	basicLightingShader = std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"BasicLightingPixelShader.cso").c_str());
	transparencyShader = std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"TransparencyPixelShader.cso").c_str());
}

void Game::SetLights() {
	Light directionalLight1 = {};
	directionalLight1.type = LIGHT_TYPE_DIRECTIONAL;
	directionalLight1.color = XMFLOAT3(1, 1, 1);
	directionalLight1.direction = XMFLOAT3(1, 0, 0);
	directionalLight1.intensity = 0.6f;

	Light directionalLight2 = {};
	directionalLight2.type = LIGHT_TYPE_DIRECTIONAL;
	directionalLight2.color = XMFLOAT3(1, 1, 1);
	directionalLight2.direction = XMFLOAT3(-1, -1, 0);
	directionalLight2.intensity = 0.9f;

	Light directionalLight3 = {};
	directionalLight3.type = LIGHT_TYPE_DIRECTIONAL;
	directionalLight3.color = XMFLOAT3(1, 1, 1);
	directionalLight3.direction = XMFLOAT3(0, 0, 1);
	directionalLight3.intensity = 1.3f;

	Light pointLight1 = {};
	pointLight1.type = LIGHT_TYPE_POINT;
	pointLight1.color = XMFLOAT3(1, 1, 1);
	pointLight1.position = XMFLOAT3(-4, 1.5, 0);
	pointLight1.intensity = 1;
	pointLight1.range = 3;

	Light pointLight2 = {};
	pointLight2.type = LIGHT_TYPE_POINT;
	pointLight2.color = XMFLOAT3(1, 1, 1);
	pointLight2.position = XMFLOAT3(6, -2, -2);
	pointLight2.intensity = 0.8f;
	pointLight2.range = 3;

	lights.push_back(directionalLight1);
	lights.push_back(directionalLight2);
	lights.push_back(directionalLight3);
	lights.push_back(pointLight1);
	lights.push_back(pointLight2);
}

// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/metalhatch_albedo.tif").c_str(), 0, metalHatchTex.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/metalhatch_roughness.tif").c_str(), 0, metalHatchRoughness.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/metalhatch_normal.tif").c_str(), 0, metalHatchNormal.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/metalhatch_metalness.tif").c_str(), 0, metalHatchMetalness.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/asteroid_albedo.tif").c_str(), 0, asteroidTex.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/asteroid_roughness.tif").c_str(), 0, asteroidRoughness.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/asteroid_normal.tif").c_str(), 0, asteroidNormal.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/asteroid_metalness.png").c_str(), 0, asteroidMetalness.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/scratched_albedo.png").c_str(), 0, scratchedTex.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/scratched_roughness.png").c_str(), 0, scratchedRoughness.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/scratched_normal.png").c_str(), 0, scratchedNormal.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/scratched_metalness.png").c_str(), 0, scratchedMetalness.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/copper_albedo.tif").c_str(), 0, copperTex.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/copper_roughness.tif").c_str(), 0, copperRoughness.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/copper_normal.tif").c_str(), 0, copperNormal.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/copper_metalness.png").c_str(), 0, copperMetalness.GetAddressOf());
	CreateDDSTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/SunnyCubeMap.dds").c_str(), 0, skyBoxTex.GetAddressOf());

	D3D11_SAMPLER_DESC desc = {};
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.Filter = D3D11_FILTER_ANISOTROPIC;
	desc.MaxAnisotropy = 8; //1-16, higher is slower -- possibly adjust this later
	desc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&desc, samplerState.GetAddressOf());

	metalHatchMaterial = new Material(XMFLOAT4(1, 1, 1, 1), vertexShader, basicLightingShader);
	asteroidMaterial = new Material(XMFLOAT4(1, 1, 1, 1), vertexShader, basicLightingShader);
	scratchedMaterial = new Material(XMFLOAT4(1, 1, 1, 1), vertexShader, basicLightingShader);
	copperMaterial = new Material(XMFLOAT4(1, 1, 1, 1), vertexShader, basicLightingShader);
	transparentMaterial = new Material(XMFLOAT4(1, 0, 0, 0.1f), vertexShader, transparencyShader, 0.1f);

	metalHatchMaterial->AddTextureSRV("Albedo", metalHatchTex);
	metalHatchMaterial->AddTextureSRV("RoughnessMap", metalHatchRoughness);
	metalHatchMaterial->AddTextureSRV("NormalMap", metalHatchNormal);
	metalHatchMaterial->AddTextureSRV("MetalnessMap", metalHatchMetalness);
	metalHatchMaterial->AddSampler("Sampler", samplerState); //can't call ut SamplerState because thats an HLSL keyword
	asteroidMaterial->AddTextureSRV("Albedo", asteroidTex);
	asteroidMaterial->AddTextureSRV("RoughnessMap", asteroidRoughness);
	asteroidMaterial->AddTextureSRV("NormalMap", asteroidNormal);
	asteroidMaterial->AddTextureSRV("MetalnessMap", asteroidMetalness);
	asteroidMaterial->AddSampler("Sampler", samplerState);
	scratchedMaterial->AddTextureSRV("Albedo", scratchedTex);
	scratchedMaterial->AddTextureSRV("RoughnessMap", scratchedRoughness);
	scratchedMaterial->AddTextureSRV("NormalMap", scratchedNormal);
	scratchedMaterial->AddTextureSRV("MetalnessMap", scratchedMetalness);
	scratchedMaterial->AddSampler("Sampler", samplerState);
	copperMaterial->AddTextureSRV("Albedo", copperTex);
	copperMaterial->AddTextureSRV("RoughnessMap", copperRoughness);
	copperMaterial->AddTextureSRV("NormalMap", copperNormal);
	copperMaterial->AddTextureSRV("MetalnessMap", copperMetalness);
	copperMaterial->AddSampler("Sampler", samplerState);
	
	sphereMesh = new Mesh(GetFullPathTo("../../Assets/Models/sphere.obj").c_str(), device, context);
	cubeMesh = new Mesh(GetFullPathTo("../../Assets/Models/cube.obj").c_str(), device, context);
	helixMesh = new Mesh(GetFullPathTo("../../Assets/Models/helix.obj").c_str(), device, context);

	sphere1 = std::make_shared<MeshEntity>(sphereMesh, transparentMaterial);
	sphere1->GetTransform()->SetPosition(-6, 0, 0);
	cube = std::make_shared<MeshEntity>(cubeMesh, metalHatchMaterial);
	cube->GetTransform()->SetPosition(-2, 0, 0);
	helix = std::make_shared<MeshEntity>(helixMesh, copperMaterial);
	helix->GetTransform()->SetPosition(2, 0, 0);
	sphere2 = std::make_shared<MeshEntity>(sphereMesh, asteroidMaterial);
	sphere2->GetTransform()->SetPosition(6, 0, 0);
	meshEntities.push_back(sphere1);
	meshEntities.push_back(cube);
	meshEntities.push_back(helix);
	meshEntities.push_back(sphere2);

	skyBox = new SkyBox(cubeMesh, skyBoxTex, skyBoxVertexShader, skyBoxPixelShader, samplerState, device);
}


// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();
	if(camera) camera->UpdateProjectionMatrix((float)this->width / this->height);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	for (int i = 0; i < meshEntities.size(); ++i) {
		meshEntities.at(i)->GetTransform()->Turn(-0.5f * deltaTime, 0.5f * deltaTime, 0.5f * deltaTime);
	}
	camera->Update(deltaTime);



	// Example input checking: Quit if the escape key is pressed
	if (Input::GetInstance().KeyDown(VK_ESCAPE))
		Quit();
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV.Get(), color);
	context->ClearDepthStencilView(
		depthStencilView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	// We can't do this in Material or MeshEntity because it can't be done to just any shader, just this one in particular
	basicLightingShader->SetFloat3("cameraPosition", camera->GetTransform().GetPosition());
	basicLightingShader->SetData("lights", &lights[0], sizeof(Light) * (int)lights.size());
	transparencyShader->SetFloat3("cameraPosition", camera->GetTransform().GetPosition());
	transparencyShader->SetData("lights", &lights[0], sizeof(Light) * (int)lights.size());
	skyBox->Draw(camera, context); //after drawing objects
	for (int i = 0; i < meshEntities.size(); ++i) {
		std::shared_ptr<MeshEntity> currentEntity = meshEntities.at(i);
		//should work fine without checking (just potentially unneccessary setting), does this help or hurt performance?
		currentEntity->GetMaterial()->BindResources();			
		if (currentEntity->GetMaterial()->GetPixelShader() == transparencyShader) {
			transparencyShader->SetFloat3("position", meshEntities[i]->GetTransform()->GetPosition());
		}
		currentEntity -> Draw(camera, context);
	}

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(vsync ? 1 : 0, 0);

	// Due to the usage of a more sophisticated swap chain,
	// the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());
}