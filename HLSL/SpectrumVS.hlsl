#include"Spectrum.hlsli"

Texture1D<float> Spectrum : register(t0);

VS_OUT main(uint vertexID : SV_VertexID)
{
    VS_OUT vout;

    uint binIndex = vertexID / 6;
    uint vertexIndex = vertexID % 6;

    uint3 idx = uint3(min(binIndex, 512 - 1), 0, 0);
    float amp = Spectrum.Load(idx);

    float x[2] =
    {
        (binIndex + 0) / numBins,
        (binIndex + 1) / numBins
    };

    float2 quad[6] = {
        float2(x[0], 0),
        float2(x[1], 0),
        float2(x[0], amp),
        float2(x[0], amp),
        float2(x[1], 0),
        float2(x[1], amp)
    };

    float2 pos = float2(quad[vertexIndex].x, quad[vertexIndex].y) * 2.0f - 1.0f;
    vout.pos = float4(pos, 0, 1);
    vout.uv = float2((float)binIndex, 0.5f);    //  quad[vertexIndex].y);

    return vout;
}