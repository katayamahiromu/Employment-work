#include "SkinnedMesh.hlsli"
#include "physical_based_rendering_functions.hlsli"

SamplerState samplerState : register(s0);
Texture2D textureMaps[4] : register(t0);
Texture2D ramp : register(t3);

//--------------------------------------------
// ランプシェーディング
//--------------------------------------------
// tex:ランプシェーディング用テクスチャ
// samp:ランプシェーディング用サンプラステート
// N:法線(正規化済み)
// L:入射ベクトル(正規化済み)
// K:反射率
float3 CalcRempDiffuse(Texture2D tex, SamplerState samp, float3 N, float3 L, float3 K)
{
    float D = saturate(dot(N, -L) * 0.5f + 0.5f);
    float Ramp = tex.Sample(samp, float2(D, 0.5f)).r;
    return K * Ramp;
}


//--------------------------------------------
// リムライト
//--------------------------------------------
// N:法線(正規化済み)
// E:視点方向ベクトル(正規化済み)
// L:入射ベクトル(正規化済み)
// C :ライト色
// RimPower : リムライトの強さ
float3 CalcRimLight(float3 N, float3 E, float3 L, float3 C, float RimPower = 0.5f)
{
    float3 rim = 1.0f - saturate(dot(N, -E));
    return C * pow(rim, RimPower) * saturate(dot(L, -E));
}


float4 main(GS_OUT input, bool is_front_face : SV_IsFrontFace) : SV_TARGET
{
    //  輪郭線の場合黒を出す
   if (input.is_outline)
   {
       return float4(lineColor, 1);
   }

    VS_OUT pin = input.vs;

    //ベースカラーの取得
    float4 color = textureMaps[0].Sample(samplerState, pin.texcoord);
    //color.rgb = ComputeRamp(ramp, samplerState, color.rgb);
    color.rgb = pow(color.rgb, GammaFactor);
    float alpha = color.a;

    //接空間の基底ベクトルの整備
    float3 N = normalize(pin.world_normal.xyz);
    float3 T = normalize(pin.world_tangent.xyz);
    float sigma = pin.world_tangent.w;
    T = normalize(T - N * dot(N, T));
    float3 B = normalize(cross(N, T) * sigma);

    //	裏面描画の場合は反転しておく
    if (is_front_face == false)
    {
        T = -T;
        B = -B;
        N = -N;
    }


    float4 normal = textureMaps[1].Sample(samplerState, pin.texcoord);
    normal = (normal * 2.0) - 1.0;
    N = normalize((normal.x * T) + (normal.y * B) + (normal.z * N));

    float3 L = normalize(-light_direction.xyz);

    float3 V = normalize(camera_position.xyz - pin.world_position.xyz);

    float3 diffuse = (float3) 0, specular = (float3) 0;

    float metalness = 1.0;

    //	入射光のうち拡散反射になる割合
    float3 diffuse_reflectance = lerp(color.rgb, 0.0f, metalness);

    //	垂直反射時のフレネル反射率(非金属でも最低4%は鏡面反射する)
    float3 F0 = lerp(0.04f, color.rgb, metalness);

    float roughness = 0.0;

    //ライトの色
    float3 LC = float3(0.5f,0.5f,0.5f);
    DirectBRDF(diffuse_reflectance, F0, N, V, L, LC, roughness, diffuse, specular);

    //// Rampトーン変換（スタイライズ）
   /* float3 toonDiffuse = ComputeRamp(ramp, samplerState, diffuse) * color.rgb;
    float3 toonSpecular = ComputeRamp(ramp, samplerState, specular) * color.rgb;*/


    //間接光の簡易実装
    float3 ambient = color.rgb * 0.7;
    diffuse = CalcRempDiffuse(ramp, samplerState, N, L, diffuse + ambient);
    //色生成
    //白くなりすぎるのである程度の数字で割って調整
    float3 ReturnColor = (diffuse + specular);
    //ReturnColor += CalcRimLight(N, V, L, LC, 1.0);
    //float3 ReturnColor = (diffuse + specular + color.rgb)/1.5f;
    //sRGB空間へ
    ReturnColor = pow(ReturnColor.rgb, 1.0f / GammaFactor);

    return float4(ReturnColor.rgb, color.a);
}