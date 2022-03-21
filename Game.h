#pragma once

#include "DXCore.h"
#include "Lights.h"

#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects
#include <memory>
#include <vector>

#include "SimpleShader.h"
#include "Mesh.h"
#include "MeshEntity.h"
#include "Camera.h"

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

private:

	// Should we use vsync to limit the frame rate?
	bool vsync;

	// Shaders and shader-related constructs
	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimplePixelShader> funkyPixelShader;
	std::shared_ptr<SimplePixelShader> basicLightingShader;
	std::shared_ptr<SimpleVertexShader> vertexShader;

	std::shared_ptr<Camera> camera;

	Mesh* sphereMesh;
	Mesh* cubeMesh;
	Mesh* helixMesh;

	std::shared_ptr<MeshEntity> sphere1;
	std::shared_ptr<MeshEntity> sphere2;
	std::shared_ptr<MeshEntity> cube;
	std::shared_ptr<MeshEntity> helix;

	std::vector<std::shared_ptr<MeshEntity>> meshEntities;

	Material* basicCyanMaterial;
	Material* funkyMaterial;
	Material* basicLightedMaterial;
	Material* experimentalLightingMaterial;

	std::vector<Light> lights;

	DirectX::XMFLOAT3 ambientColor;

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void CreateBasicGeometry();
	void SetLights();

	// Note the usage of ComPtr below
	//  - This is a smart pointer for objects that abide by the
	//    Component Object Model, which DirectX objects do
	//  - More info here: https://github.com/Microsoft/DirectXTK/wiki/ComPtr
	

};

