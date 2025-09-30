#include"Buffer.hlsli"
struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
	float4 world_position : WORLD_POSITION;
};

CBUFFER(SKY_MAP_BUFFER,CBS_SKY_MAP)
{
	float4 camera_position;
	row_major float4x4 inverse_view_projection;
}