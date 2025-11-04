#include"FullScreenQuad.hlsli"

Texture2D<float> depthTex : register(t0);
SamplerState samplerState : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float depth = depthTex.Sample(samplerState, pin.texcoord);
	return float4(depth.xxx, 1.0f); //深度をグレースケールで表示
}