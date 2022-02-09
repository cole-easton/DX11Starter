#pragma once
#include "Transform.h"
#include "Mesh.h"

class MeshEntity
{
private:
	Mesh * pMesh;
	Transform transform;
public: 
	MeshEntity(Mesh * mesh);
	Mesh * GetMesh();
	Transform * const GetTransform();
	void Draw(Microsoft::WRL::ComPtr<ID3D11Buffer> constantBufferVS, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);
};

