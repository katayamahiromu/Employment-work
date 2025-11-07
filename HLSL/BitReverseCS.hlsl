#include"FFT.hlsli"

//情報入力バッファ
StructuredBuffer<float2> data_input : register(t0);

//情報書き込みバッファ
RWStructuredBuffer<float2> data_output : register(u0);

[numthreads(16, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	uint i = DTid.x;
	if (i >= N) return;

	//ビット反転
	uint j = 0;
	uint x = i;
	for (uint k = 0;k < log2N;++k)
	{
		j = (j << 1) | (x & 1);
		x >>= 1;
	}
	data_output[j] = data_input[i];
}