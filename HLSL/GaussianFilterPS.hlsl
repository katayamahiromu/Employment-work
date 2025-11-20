#include "FullScreenQuad.hlsli"

#define KERNEL_MAX 25
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer Gaussian : register(b2)
{
    float4 weights[KERNEL_MAX * KERNEL_MAX];
    float kernelSize;
    float2 texcel;
    float dummy;
}

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = texture0.Sample(sampler0, pin.texcoord);
    color.a = 1;
    
	//指定のカーネルサイズ分周囲から色を取得。CPU側で計算した重みを積和していく
    for (int i = 0; i < kernelSize * kernelSize; i++)
    {
        float2 offset = texcel * weights[i].xy;
        float weight = weights[i].z;
        color.rgb += texture0.Sample(sampler0, pin.texcoord + offset).rgb * weight;
    }
    
    return float4(color.rgb,1);
}