#ifndef __STRUCT_INCLUDES__
#define __STRUCT_INCLUDES__

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2
#define MAX_SPECULAR_EXPONENT 256.0f

// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 screenPosition	: SV_POSITION;
	float2 uv				: TEXCOORD;
	float3 normal			: NORMAL;
	float3 worldPosition	: POSITION;
};

// Struct representing a single vertex worth of data
// - This should match the vertex definition in our C++ code
// - By "match", I mean the size, order and number of members
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexShaderInput
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float3 localPosition	: POSITION;     // XYZ position
	float3 normal			: NORMAL;
	float2 uv				: TEXCOORD;
};

struct Light {
	int type				: LIGHT_TYPE;
	float3 direction		: DIRECTION;
	float range				: LIGHT_RANGE;
	float3 position			: POSITION;
	float intensity			: LIGHT_INTENSITY;
	float3 color			: COLOR;
	float spotFalloff		: LIGHT_FALLOFF;
	float3 padding			: PADDING;
};

struct LightingInfo {
	float3 normal			: NORMAL;
	float roughness			: ROUGHNESS;
	float3 worldPosition	: POSITION;
	float surfaceColor		: COLOR;
	float3 cameraPosition	: POSITION;
	float padding			: PADDING;
	float3 ambientColor		: COLOR;
};

#endif