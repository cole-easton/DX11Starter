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
#include "Skybox.h"

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
	std::shared_ptr<SimplePixelShader> basicLightingShader;
	std::shared_ptr<SimplePixelShader> transparencyShader;
	std::shared_ptr<SimplePixelShader> skyBoxPixelShader;
	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimpleVertexShader> skyBoxVertexShader;

	std::shared_ptr<Camera> camera;

	Mesh* sphereMesh;
	Mesh* cubeMesh;
	Mesh* helixMesh;

	SkyBox* skyBox;

	std::shared_ptr<MeshEntity> sphere1;
	std::shared_ptr<MeshEntity> sphere2;
	std::shared_ptr<MeshEntity> cube;
	std::shared_ptr<MeshEntity> helix;

	std::vector<std::shared_ptr<MeshEntity>> meshEntities;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalHatchTex;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalHatchRoughness;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalHatchNormal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalHatchMetalness;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> asteroidTex;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> asteroidRoughness;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> asteroidNormal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> asteroidMetalness;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> scratchedTex;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> scratchedRoughness;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> scratchedNormal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> scratchedMetalness;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> copperTex;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> copperRoughness;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> copperNormal;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> copperMetalness;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skyBoxTex;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;

	Material* metalHatchMaterial;
	Material* asteroidMaterial;
	Material* scratchedMaterial;
	Material* copperMaterial;
	Material* transparentMaterial;

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

