#include"FFT.hlsli"
//バッファ複素数　float2

//情報入力バッファ
StructuredBuffer<float2> data_input : register(t0);

//情報書き込みバッファ
RWStructuredBuffer<float2> data_output : register(u0);

[numthreads(512, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint index = DTid.x;
	if (index >= N / 2)return;

	uint halfSize = 1 << stage;
	uint stepSize = halfSize * 2;

	uint block = index / halfSize;
	uint j = index % halfSize;

	uint i0 = block * stepSize + j;
	uint i1 = i0 + halfSize;

	float angle = -2.0f * 3.14159265f * j / stepSize;
	float2 w = float2(cos(angle), sin(angle));

	float2 u = data_input[i0];
	float2 v = data_input[i1];

	//複素数乗算v*w
	float2 vw = float2(
		v.x * w.x - v.y * w.y,
		v.x * w.y + v.y * w.x
		);

	data_output[i0] = u + vw;
	data_output[i1] = u - vw;
}