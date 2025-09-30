//í∏ì_èÓïÒ

struct VS_IN
{
	float3 position : POSITION;
	float2 size : TEXCOORD;
	float4 color : COLOR;
	float4 param : PARAMETER;
};

struct GS_IN
{
	float3 position : POSITION;
	float2 size : TEXCOORD;
	float4 color : COLOR;
	float4 param : PARAMETER;
};

struct PS_IN
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
};

cbuffer GEOMETRY_PARTICLE_DATA : register(b3)
{
	float2 size;
	float2 dummy;
}

cbuffer GEOMETRY_PARTICLE_BUFFER : register(b4)
{
	row_major float4x4 view_matrix;
	row_major float4x4 projection_matrix;
}