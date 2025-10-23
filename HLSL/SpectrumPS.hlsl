#include"Spectrum.hlsli"

Texture1D<float> Spectrum : register(t0);
SamplerState samp : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float2 uv = pin.uv;
    uint dim; 
    Spectrum.GetDimensions(dim);
    uint3 idx = uint3(min(uint(uv.x + 0.1f), dim - 1), 0, 0);
    float amp = Spectrum.Load(idx);

    float alpha = (uv.y <= amp) ? 1.0f : 0.05f;
    float3 col = lerp(float3(0.0,0.2,1.0), float3(1.0,0.2,0.0), amp);
    return float4(col, alpha);
}