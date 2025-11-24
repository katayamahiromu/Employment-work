#pragma once
#include<vector>

static inline void pushInt16LE(std::vector<uint8_t>& out, int16_t v)
{
    out.push_back(static_cast<uint8_t>(v & 0xFF));
    out.push_back(static_cast<uint8_t>((v >> 8) & 0xFF));
}

//PCM16の形でリトルエンディアンでデータを格納
inline static std::vector<UINT8> EncodePCM16LE(std::vector<int>& buf)
{
    // 最大振幅を求める（正規化用）
    int maxAmp = 0;
    for (auto v : buf) maxAmp = max(maxAmp, std::abs(v));

    // ゼロ除算回避
    float norm = (maxAmp > 0) ? (32767.0f / static_cast<float>(maxAmp)) : 1.0f;

    // 出力バイト列に格納（PCM16 LE）
    size_t maxSamples = buf.size();
    std::vector<UINT8> data;
    data.resize(maxSamples * 2);
    for (size_t i = 0; i < maxSamples; ++i) {
        float scaled = buf[i] * norm;
        // クリッピング
        if (scaled > 32767.0f) scaled = 32767.0f;
        if (scaled < -32768.0f) scaled = -32768.0f;
        int16_t s = static_cast<int16_t>(lround(scaled));
        data[i * 2] = static_cast<uint8_t>(s & 0xFF);
        data[i * 2 + 1] = static_cast<uint8_t>((s >> 8) & 0xFF);
    }

    return data;
}