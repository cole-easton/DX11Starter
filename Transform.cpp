#include "Transform.h"

using namespace DirectX;

Transform::Transform() : Transform(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1))
{
}

Transform::Transform(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale)
{
	this->position = position;
	this->rotation = rotation;
	this->scale = scale;
	dirty = false;
}

Transform::Transform(float posX, float posY, float posZ, float pitch, float yaw, float roll, float scaleX, float scaleY, float scaleZ)
	: Transform(XMFLOAT3(posX, posY, posZ), XMFLOAT3(pitch, yaw, roll), XMFLOAT3(scaleX, scaleY, scaleZ))
{
}

void Transform::SetPosition(float x, float y, float z)
{
	position = XMFLOAT3(x, y, z);
	dirty = true;
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
	rotation = XMFLOAT3(pitch, yaw, roll);
	dirty = true;
}

void Transform::SetScale(float x, float y, float z)
{
	scale = XMFLOAT3(x, y, z);
	dirty = true;
}

DirectX::XMFLOAT3 Transform::GetPosition()
{
	return position;
}

DirectX::XMFLOAT3 Transform::GetRotation()
{
	return rotation;
}

DirectX::XMFLOAT3 Transform::GetScale()
{
	return scale;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
	if (dirty) {
		XMMATRIX trans = XMMatrixTranslation(position.x, position.y, position.z);
		XMMATRIX rot = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
		XMMATRIX scaling = XMMatrixScaling(scale.x, scale.y, scale.z);
		XMStoreFloat4x4(&world, XMMatrixMultiply(XMMatrixMultiply(scaling, rot), trans) );
	}
	return world;
}

DirectX::XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
	return DirectX::XMFLOAT4X4();
}

void Transform::Translate(float x, float y, float z)
{
	XMStoreFloat3(&position, XMLoadFloat3(&position) + XMVectorSet(x, y, z, 0));
}

void Transform::Turn(float pitch, float yaw, float roll)
{
	XMStoreFloat3(&rotation, XMLoadFloat3(&rotation) + XMVectorSet(pitch, yaw, roll, 0));
}

void Transform::Scale(float x, float y, float z)
{
	XMStoreFloat3(&scale, XMLoadFloat3(&scale) + XMVectorSet(x, y, z, 0));

}
