cbuffer FFTParams : register(b10)
{
	uint N;
	uint log2N;
	uint stage;
	int dummy;
};