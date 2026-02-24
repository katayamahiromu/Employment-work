#include "FullScreenQuad.hlsli"

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer Rainline : register(b2)
{
    float rainIntensity; // 雨の強度
    float rainSpeed; // 落下速度
    float rainScale; // 雨の密度（UVスケール）
    float time; //時間
};

//ランダム生成
float hash(float2 p)
{
    p = frac(p * float2(123.34, 345.45));
    p += dot(p, p + 34.345);
    return frac(p.x * p.y);
}

float rainStreak(float2 uv, float t)
{
    // UV を拡大して雨の密度を調整
    uv *= rainScale;

    // 落下
    uv.y -= t * rainSpeed;

    // 風で揺らす
    uv.x += sin(uv.y * 5.0) * 0.02;

    // タイル化
    float2 gv = frac(uv) - 0.5;
    float2 id = floor(uv);

    // ランダム値
    float n = hash(id);
    
    //縦方向にストリークを伸ばす
    gv.y *= 0.002;
    
    // 雨粒の中心をランダムにずらす
    float d = length(gv + float2(0.0, -n * 0.8));

    // 細い線にする
    return smoothstep(0.05, 0.02, d) * n;
}

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = texture0.Sample(sampler0, pin.texcoord);
    
    //雨のストリーク生成
    float r = rainStreak(pin.texcoord, time) * rainIntensity;
    
    //雨の色
    color.rgb += float3(r,r,r);
    return color;
}