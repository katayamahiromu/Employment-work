#include "FullScreenQuad.hlsli"

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = texture0.Sample(sampler0, pin.texcoord);
    
    //‹P“x‚ğ‹‚ß‚é
    float luminance = dot(color.rgb, float3(0.299, 0.587, 0.114));
    
    color.a = step(0.05, luminance);

    return color;
}