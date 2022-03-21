#include "Game.h"
#include "Vertex.h"
#include "Input.h"
#include "Lights.h"

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
	delete basicCyanMaterial;
	delete funkyMaterial;
	delete basicLightedMaterial;
	delete sphereMesh;
	delete cubeMesh;
	delete helixMesh;

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
	camera = std::make_shared<Camera>(Transform(0, 0, -10, 0, 0, 0, 1, 1, 1), (float)this->width / this->height);
	ambientColor = XMFLOAT3(0.15, 0.15, 0.25);
	
	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
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
	pixelShader = std::make_shared<SimplePixelShader>(device, context,GetFullPathTo_Wide(L"PixelShader.cso").c_str());
	funkyPixelShader = std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"FunkyPixelShader.cso").c_str());
	basicLightingShader = std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"BasicLightingPixelSHader.cso").c_str());
}



// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	basicCyanMaterial = new Material(XMFLOAT4(0, 0.8, 0.8, 1), 0.15, vertexShader, pixelShader);
	basicLightedMaterial = new Material(XMFLOAT4(0.9, 0.9, 0.9, 1), 0.15, vertexShader, basicLightingShader);
	funkyMaterial = new Material(XMFLOAT4(1, 1, 1, 1), 1, vertexShader, funkyPixelShader);

	sphereMesh = new Mesh(GetFullPathTo("../../Assets/Models/sphere.obj").c_str(), device, context);
	cubeMesh = new Mesh(GetFullPathTo("../../Assets/Models/cube.obj").c_str(), device, context);
	helixMesh = new Mesh(GetFullPathTo("../../Assets/Models/helix.obj").c_str(), device, context);

	sphere1 = std::make_shared<MeshEntity>(sphereMesh, basicLightedMaterial);
	sphere1->GetTransform()->SetPosition(-6, 0, 0);
	cube = std::make_shared<MeshEntity>(cubeMesh, basicLightedMaterial);
	cube->GetTransform()->SetPosition(-2, 0, 0);
	helix = std::make_shared<MeshEntity>(helixMesh, basicLightedMaterial);
	helix->GetTransform()->SetPosition(2, 0, 0);
	sphere2 = std::make_shared<MeshEntity>(sphereMesh, basicLightedMaterial);
	sphere2->GetTransform()->SetPosition(6, 0, 0);
	meshEntities.push_back(sphere1);
	meshEntities.push_back(cube);
	meshEntities.push_back(helix);
	meshEntities.push_back(sphere2);
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
		meshEntities.at(i)->GetTransform()->Turn(-2 * deltaTime, 2 * deltaTime, 2 * deltaTime);
	}
	camera->Update(deltaTime);

	Light directionalLight1 = {};
	directionalLight1.type = LIGHT_TYPE_DIRECTIONAL;
	directionalLight1.color = XMFLOAT3(1, 1, 1);
	directionalLight1.direction = XMFLOAT3(1, 1, 1);
	directionalLight1.intensity = 1;


	Light directionalLight2 = {
		LIGHT_TYPE_DIRECTIONAL,
		XMFLOAT3(-1, -1, 0),//direction
		0,
		XMFLOAT3(),
		1, //intensity
		XMFLOAT3(1, 1, 0) //color
	};
	pixelShader->SetData(
		"directionalLight1", // The name of the (eventual) variable in the shader
		&directionalLight1, // The address of the data to set
		sizeof(Light));

	pixelShader->SetData(
		"directionalLight2", 
		&directionalLight2, 
		sizeof(Light));

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
	for (int i = 0; i < meshEntities.size(); ++i) {
		std::shared_ptr<MeshEntity> currentEntity = meshEntities.at(i);
		//should work fine without checking (just potentially unneccessary setting), does this help or hurt performance?
		if (currentEntity->GetMaterial()->GetPixelShader() == basicLightingShader) {
			currentEntity->GetMaterial()->GetPixelShader()->SetFloat("roughness", currentEntity->GetMaterial()->GetRoughness());
			currentEntity->GetMaterial()->GetPixelShader()->SetFloat3("ambientColor", ambientColor);
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