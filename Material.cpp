#include "Material.h"

// roughness must be within the range 0 - 1
Material::Material(DirectX::XMFLOAT4 colorTint, float roughness, std::shared_ptr<SimpleVertexShader> vertexShader, std::shared_ptr<SimplePixelShader> pixelShader)
{
	this->colorTint = colorTint;
	this->roughness = roughness;
	this->vertexShader = vertexShader;
	this->pixelShader = pixelShader;
}

DirectX::XMFLOAT4 Material::GetColorTint()
{
	return colorTint;
}

float Material::GetRoughness()
{
	return roughness;
}

std::shared_ptr<SimpleVertexShader> Material::GetVertexShader()
{
	return vertexShader;
}

std::shared_ptr<SimplePixelShader> Material::GetPixelShader()
{
	return pixelShader;
}

void Material::AddTextureSRV(std::string shaderName, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv)
{
	textureSRVs.insert({ shaderName, srv });
}

void Material::AddSampler(std::string shaderName, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler)
{
	samplers.insert({ shaderName, sampler });
}

void Material::BindResources()
{
	for (auto& t : textureSRVs) { 
		pixelShader->SetShaderResourceView(t.first.c_str(), t.second);
	}
	for (auto& s : samplers) {
		pixelShader->SetSamplerState(s.first.c_str(), s.second); 
	}

}
