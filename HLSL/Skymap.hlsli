struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
	float2 texcoord : TEXCOORD;
	float4 world_position : WORLD_POSITION;
};

cbuffer SKY_MAP_BUFFER:register(b3)
{
	float4 camera_position;
	row_major float4x4 inverse_view_projection;
}