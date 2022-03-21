#ifndef __LIGHTING_INCLUDES__
#define __LIGHTING_INCLUDES__
#include "StructIncludes.hlsli"

//Precondition: normal and dirToLight are both normalized
float3 diffuseColor(Light light, float3 normal, float3 worldPosition) {
	float3 direction = float3(0, 0, 0);
	if (light.type == LIGHT_TYPE_DIRECTIONAL) {
		direction = normalize(light.direction);
	}
	else if (light.type = LIGHT_TYPE_POINT) {
		direction = normalize(worldPosition - light.position);
	}
	return saturate(dot(normal, -direction)).rrr * light.color;
}

float attenuate(Light light, float3 worldPos)
{
	float dist = distance(light.position, worldPos);
	float att = saturate(1.0f - (dist * dist / (light.range * light.range)));
	return att * att;
}

float3 specularColor(Light light, float3 normal, float3 worldPosition, float3 cameraPosition, float roughness) {
	float3 direction = float3(0, 0, 0);
	if (light.type == LIGHT_TYPE_DIRECTIONAL) {
		direction = normalize(light.direction);
	}
	else if (light.type = LIGHT_TYPE_POINT) {
		direction = normalize(worldPosition - light.position);
	}
	float specExponent = (1 - roughness) * MAX_SPECULAR_EXPONENT;
	float spec = 0;
	if (specExponent > 0.05) {
		float3 V = normalize(cameraPosition - worldPosition); //vector to camera
		float3 R = reflect(direction, normal); //direction of light towards camera
		spec = pow(saturate(dot(R, V)), specExponent);
	}
	return spec;
}

float3 calculateTotalLighting(Light light, LightingInfo info) {
	float3 result = light.intensity.rrr*(diffuseColor(light, info.normal, info.worldPosition) * info.surfaceColor + specularColor(light, info.normal, info.worldPosition, info.cameraPosition, info.roughness)) + info.ambientColor;
	if (light.type == LIGHT_TYPE_POINT) {
		result *= attenuate(light, info.worldPosition).rrr;
	}
	return result;
}

#endif