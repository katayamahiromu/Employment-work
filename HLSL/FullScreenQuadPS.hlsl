#include"FullScreenQuad.hlsli"


SamplerState sampler_states : register(s0);
Texture2D texture_map : register(t0);
float4 main(VS_OUT pin) : SV_TARGET
{
	 return texture_map.Sample(sampler_states, pin.texcoord);
}