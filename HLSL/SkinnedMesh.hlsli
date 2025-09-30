#include"Buffer.hlsli"

struct VS_IN
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
	float4 bone_weights : WEIGHTS;
	uint4 bone_indices : BONES;
};

struct VS_OUT
{
	float4 position : SV_POSITION;
    float4 world_position : POSITION;
	float2 texcoord : TEXCOORD;
    float4 world_normal : NORMAL;
    float4 world_tangent : TANGENT;
	float4 color : COLOR;
};

struct GS_OUT
{
    VS_OUT vs;
    bool is_outline : IS_OUTLINE; //輪郭線判定用
};

static const int MAX_BONES = 256;
cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
    row_major float4x4 world;
    float4 material_color;
    row_major float4x4 bone_transforms[MAX_BONES];
};

CBUFFER(SCENE_CONSTANT_BUFFER, CBS_SCENE_CONSTANTS)
{
    row_major float4x4 view_projection;
    float4 light_direction;
    float4 camera_position;

    //アウトライン用
    float lineSize;
    float3 lineColor;
};



