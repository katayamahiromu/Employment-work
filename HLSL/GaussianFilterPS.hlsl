#include "FullScreenQuad.hlsli"

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer Gaussian : register(b2)
{
    float2 texelSize;
    float offset;
    int sampleCount; //サンプルのカウント
    
    float2 directions[8]; // サンプリング方向
}

float4 main(VS_OUT pin) : SV_TARGET
{
    float2 uv = pin.texcoord;
    float4 sum = 0;
    
    //指定方向から色を算出
    [unroll]
    for (int i = 0; i < sampleCount; ++i)
    {
        float2 o = directions[i] * offset * texelSize;
        sum += texture0.Sample(sampler0, uv + o);
    }

    //均した色を返す
    return sum / sampleCount;
}