cbuffer specturm : register(b10)
{
	float numBins;
	float viewWidth;
	float viewHeight;
	float dummy;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
//	nointerpolation float2 uv : TEXCOORD0;
	float2 uv : TEXCOORD0;
};
