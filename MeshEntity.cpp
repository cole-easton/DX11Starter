#include <DirectXMath.h>
#include "MeshEntity.h"
#include "BufferStructs.h"

using namespace DirectX;

MeshEntity::MeshEntity(Mesh* mesh)
{
	pMesh = mesh;
	transform = Transform();
}

Mesh* MeshEntity::GetMesh()
{
	return pMesh;
}

Transform* const MeshEntity::GetTransform()
{
	return &transform;
}

void MeshEntity::Draw(std::shared_ptr<Camera> camera, Microsoft::WRL::ComPtr<ID3D11Buffer> constantBufferVS, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
	VertexShaderExternalData vsData;
	vsData.colorTint = XMFLOAT4(1, 0.5f, 0.5f, 1);
	vsData.worldMatrix = transform.GetWorldMatrix();
	vsData.viewMatrix = camera->GetViewMatrix();
	vsData.projectionMatrix = camera->GetProjectionMatrix();
	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	context->Map(constantBufferVS.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	memcpy(mappedBuffer.pData, &vsData, sizeof(vsData));
	context->Unmap(constantBufferVS.Get(), 0);
	context->VSSetConstantBuffers(0, 1, constantBufferVS.GetAddressOf());
	pMesh->Draw();
}
