#include "FullScreenQuad.hlsli"

cbuffer MASK : register(b2)
{
    float dissolveThreshold; //ディゾルブ量
    float edgeThreshold; //緑の閾値
    float time; //時間
    float flickerFreq; //炎ゆらぎ周波数
    
    float4 innerColor;
    float4 outerColor;
};

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

Texture2D maskTexture : register(t1);
Texture2D noiseTexture : register(t2);

float4 main(VS_OUT pin) : SV_TARGET
{
    float2 uv = pin.texcoord;

    // --- 炎ノイズを動かす ---
    float2 noiseUV = uv + float2(0.0, -time * 0.25);
    float noise = noiseTexture.Sample(sampler0, noiseUV).r;

    
    // --- sin でゆらぎを加える ---
    float flicker = 0.5 + 0.5 * sin(time * flickerFreq);
    noise += flicker * 0.1; // ゆらぎ幅

    // --- マスクをノイズと合成 ---
    float mask = maskTexture.Sample(sampler0, uv).r + noise * 0.5;

    // --- ディゾルブ判定 ---
    float alpha = step(mask, dissolveThreshold);

    // --- 炎エッジ部分の計算（なめらかに） ---
    float edge = smoothstep(dissolveThreshold, dissolveThreshold, mask);

    // --- 炎の色補間（外側→内側） ---
    float3 fireColor = lerp(outerColor.rgb, innerColor.rgb, edge);

    // --- 元テクスチャ取得 ---
    float4 baseColor = texture0.Sample(sampler0, uv);

    // --- 発光エッジを加算ブレンド風に強調 ---
    float edgeGlow = (1.0 - edge) * (1.0 - dissolveThreshold);
    baseColor.rgb += fireColor * edgeGlow * 2.0; // 加算ブレンド相当

    // --- アルファ減衰（燃え尽きる） ---
    baseColor.a *= alpha;

    // --- 少しガンマ補正でトーン調整 ---
    baseColor.rgb = pow(baseColor.rgb, 1.0 / 2.2);

    // --- 完全に透明なら破棄 ---
    clip(baseColor.a - 0.01f);

    return baseColor;
}