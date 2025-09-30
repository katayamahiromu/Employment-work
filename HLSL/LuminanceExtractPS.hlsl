#include "FullScreenQuad.hlsli"

cbuffer LUMINANCE_EXTRACT : register(b2)
{
    float threshold; // ���P�x���o�̂��߂�臒l
    float intensity; // �u���[���̋��x
    float2 dummy;
};

Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

//RGB�F��Ԃ̐��l����P�x�l�ւ̕ϊ��֐�
//rgb:RGB�F��Ԃ̐��l
float RGB2Luminance(float3 rgb)
{
    static const float3 luminance_value = float3(0.299f, 0.587f, 0.114f);
    return dot(luminance_value, rgb);
}

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = texture0.Sample(sampler0, pin.texcoord);
    float alpha = color.a;
    // RGB > �P�x�l�ɕϊ�
    float luminance = RGB2Luminance(color.rgb);

    // 臒l�Ƃ̍����Z�o
    float contribution = max(0, luminance - threshold);

    // �o�͂���F��␳����
    color.rgb *= contribution / luminance;
    color.rgb *= intensity;
    return float4(color);
}
