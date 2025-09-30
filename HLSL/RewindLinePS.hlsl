#include "FullScreenQuad.hlsli"

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer RewindLine:register(b2)
{
	float time;
	float2 screenSize;
	float dummy;
}

float4 main(VS_OUT pin) : SV_TARGET
{
	// === 横方向の歪み ===
	float2 uv = pin.texcoord;
	float offset = sin(uv.y * 50.0 + time * 10.0) * 0.005;
	float4 color = texture0.Sample(sampler0, uv + float2(offset, 0));

	//スキャンライン
	float scanlineg = sin(uv.y * screenSize.y * 3.1415 * 400.0);
	color.rgb *= 0.9 + 0.1 * scanlineg;

	//ノイズ
	float noise = frac(sin(dot(uv + time, float2(12.9898, 78.233))) * 43758.5453);
	color.rgb += 0.02 * (noise - 0.5);


	return color;
}