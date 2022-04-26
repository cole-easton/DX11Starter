#include "StructIncludes.hlsli"
#include "LightingIncludes.hlsli"

cbuffer externalData : register(b0) {
	float4 color;
	float roughness;
	float3 position; //the position of the sphere
	float3 cameraPosition;
	Light lights[5];
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
	float3 camToPoint = input.worldPosition - cameraPosition;
	float3 pointToCenter = position - input.worldPosition;
	float3 camToCenter = position - cameraPosition;
	float radius = 2 * length(pointToCenter);
	camToPoint = normalize(camToPoint);
	pointToCenter = normalize(pointToCenter);
	float d = dot(camToPoint, pointToCenter);
	//float thickness = sqrt(1 - d*d) * d * radius;
	float thickness = 2 * (dot(camToPoint, camToCenter)/length(camToPoint) - length(camToPoint));
	LightingInfo info;
	info.normal = normalize(input.normal);
	info.roughness = roughness;
	info.metalness = 0; //transparent objects are never metallic
	info.worldPosition = input.worldPosition;
	info.cameraPosition = cameraPosition;
	info.surfaceColor = color.rgb;

	float3 pixelColor = 0;
	for (int i = 0; i < 5; i++) {
		pixelColor += calculateTotalLighting(lights[i], info);

	}
	return float4(pow(thickness/40, 1.0f / 2.2f).rrr, 1);

}