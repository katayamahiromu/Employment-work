#include "geometry_particle.hlsli"

Texture2D color_map : register(t0);
SamplerState sampler0 : register(s0);

float4 main(PS_IN pin) : SV_TARGET
{
    return color_map.Sample(sampler0, pin.texcoord) * pin.color;
}