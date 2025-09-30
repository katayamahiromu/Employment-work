#include"FullScreenQuad.hlsli"

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

float3 ToneMapACES(float3 color)
{
    // Narkowicz 2015 - ACES Filmic Approximation
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;

    return saturate((color * (a * color + b)) / (color * (c * color + d) + e));
}

float4 main(VS_OUT pin) :SV_TARGET
{
    float3 hdrColor = texture0.Sample(sampler0,pin.texcoord).rgb;

    float exposure = 1.0;
    hdrColor *= exposure;

    float3 ldrColor = ToneMapACES(hdrColor);

    return float4(ldrColor, 1.0);
}