#include"Skymap.hlsli"

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
	//pin.world_positionのワールド座標を用いてピクセル毎の視線ベクトルを求める
	float3 E = normalize(pin.world_position.xyz - camera_position.xyz);

	static const float PI = 3.14159265f;

	float latitude = (1.0f / (2.0f * PI)) * atan2(E.z, E.x) + 0.5f;
	float longitude = (1.0f / PI) * atan2(E.y, length(E.xz)) + 0.5f;
	
	return texture0.Sample(sampler0, float2(1.0f - saturate(latitude), 1.0f - saturate(longitude)));
}