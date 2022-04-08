#include "StructIncludes.hlsli"
#include "LightingIncludes.hlsli"

cbuffer externalData : register(b0) {
	float4 colorTint;
	float3 cameraPosition;
	float roughness;
	float3 ambientColor;
	Light lights[5];
}

Texture2D SurfaceTexture : register(t0);
Texture2D RoughnessTexture : register(t1);
Texture2D NormalMap : register(t2);
SamplerState Sampler : register(s0);

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	float3 unpackedNormal = NormalMap.Sample(Sampler, input.uv/2).rgb * 2 - 1;

	input.normal = normalize(input.normal);
	input.tangent = normalize(input.tangent);
	input.tangent = normalize(input.tangent - input.normal * dot(input.tangent, input.normal)); // Gram-Schmidt assumes T&N are normalized!
	float3 bitangent = cross(input.tangent, input.normal);
	float3x3 TBN = float3x3(input.tangent, bitangent, input.normal);
	input.normal = mul(unpackedNormal, TBN);

	LightingInfo info;
	info.normal = input.normal;
	info.roughness = roughness;
	info.specularMultiplier = 1-RoughnessTexture.Sample(Sampler, input.uv/2).r;
	info.worldPosition = input.worldPosition;
	info.cameraPosition = cameraPosition;
	info.surfaceColor = colorTint * SurfaceTexture.Sample(Sampler, input.uv/2).rgb;

	float3 pixelColor = ambientColor;
	for (int i = 0; i < 5; i++) {
		pixelColor += calculateTotalLighting(lights[i], info);

	}
	return float4(pixelColor, 1);

}