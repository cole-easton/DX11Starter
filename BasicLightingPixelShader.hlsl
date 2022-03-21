#include "StructIncludes.hlsli"

cbuffer externalData : register(b0) {
	float4 colorTint;
	float3 cameraPosition;
	float roughness;
	float3 ambientColor;
	Light directionalLight1;
	Light directionalLight2;
}

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
	//return colorTint * float4(ambientColor, 1);
	input.normal = normalize(input.normal);
	return float4(directionalLight1.color, 1);
}