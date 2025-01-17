#include "StructIncludes.hlsli"

cbuffer externalData : register(b0) {
	Sphere spheres[4];
}

Texture2D Pixels : register(t0);
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
	if (length(2*input.uv-1 - spheres[0].position) < 1) {
		return float4(1, 0, 0, 1);
	}
	float3 color = Pixels.Sample(Sampler, input.uv);

	return float4(color, 1);

}