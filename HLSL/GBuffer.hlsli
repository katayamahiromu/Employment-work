#ifndef _GBUFFER_HLSLI_
#define _GBUFFER_HLSLI_

// シェーディング方式を決めるためのId
static const int shading_model_unlit = 0; // ライティング無し
static const int shading_model_phong_shading = 1; // フォンシェーディング

static const int shading_model_max = 2; // 最大数

// ピクセルシェーダーへの出力用構造体
struct PSGBufferOut
{
    float4 base_color : SV_TARGET0;
    float4 normal_depth : SV_TARGET1;
};

struct GBufferData
{
    float3 base_color; //ベースカラー
    float3 w_normal;//ワールド法線
    float3 w_position;//ワールド座標
    float depth; //深度(Decode時のみ)
    int shading_model;
};

// GBufferDataにまとめた情報をピクセルシェーダーの出力用構造体に変換
PSGBufferOut EncodeGBuffer(in GBufferData data, matrix view_projection_matrix)
{
    PSGBufferOut ret = (PSGBufferOut) 0;
    ret.base_color.rgb = data.base_color;
    ret.base_color.a = 1.0f;
    
    ret.normal_depth.rgb = data.w_normal;
    float4 position = mul(float4(data.w_position, 1.0f), view_projection_matrix);
    ret.normal_depth.a = ((float) data.shading_model) / shading_model_max;
    return ret;
}

// GBufferテクスチャ受け渡し用構造体
struct PSGBufferTextures
{
    SamplerState state;
    Texture2D<float4> base_color;
    Texture2D<float4> normal_depth;
};

// ピクセルシェーダーの出力用構造体からGBufferData情報に変換
GBufferData DecodeGBuffer(PSGBufferTextures textures, float2 uv, matrix inverse_view_projection_transform)
{
    // 各テクスチャから情報を取得
    float4 base_color = textures.base_color.SampleLevel(textures.state, uv, 0);
    float4 normal_depth = textures.normal_depth.SampleLevel(textures.state, uv, 0);
    
    GBufferData ret = (GBufferData) 0;
    ret.base_color = base_color.rgb;
    ret.w_normal = normalize(normal_depth.rgb);
    ret.depth = normal_depth.a;
    float4 position = float4(uv.xy * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f), ret.depth, 1.0f);
    ret.w_position = position.xyz / position.w;
    
    ret.shading_model = (int) (base_color.a * shading_model_max + 0.5f);
    
    return ret;
}

#endif // _GBUFFER_HLSLI_