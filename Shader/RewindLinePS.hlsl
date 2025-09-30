#include"FullScreenQuad.hlsli"

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
	float2 uv = pin.texcoord;
	float4 color = texture0.Sample(sampler0,uv);

	//スキャンライン
	float scanline = sin(uv.y * screenSize.y * 3.1415 * 400.0);
	color.rgb *= 0.9 + 0.1 * scanling;

	//ノイズ
	float noise = frac(sin(dot(uv + Time, float2(12.9898, 78.233))) * 43758.5453)))
	color.rgb += 0.02 * (noise - 0.5);

	// === 横方向の歪み ===
	float offset = sin(uv.y * 50.0 + Time * 10.0) * 0.005;
	color += texture0.Sample(Sampler, uv + float2(offset, 0));

	return color;
}