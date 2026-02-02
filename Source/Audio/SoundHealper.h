#pragma once
#include<vector>
#include"ModalMode.h"
#include"../Utils/json-develop/single_include/nlohmann/json.hpp"
#include<algorithm>
#include<math.h>
#include <iostream>
#include <fstream>
#include <optional>

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
    for (auto v : buf) maxAmp = std::max<int>(maxAmp, std::abs(v));

    // ゼロ除算回避
    float norm = (maxAmp > 0) ? (32767.0f / static_cast<float>(maxAmp)) : 1.0f;

    // 出力バイト列に格納（PCM16 LE）
    size_t maxSamples = buf.size();
    std::vector<UINT8> data;
    data.resize(maxSamples * 2);
    for (size_t i = 0; i < maxSamples; ++i) {
        float scaled = buf[i] * norm;
        // クリッピング
       /* if (scaled > 32767.0f) scaled = 32767.0f;
        if (scaled < -32768.0f) scaled = -32768.0f;*/
        scaled = std::clamp(scaled, -32768.0f, 32768.0f);
        int16_t s = static_cast<int16_t>(lround(scaled));
        data[i * 2] = static_cast<uint8_t>(s & 0xFF);
        data[i * 2 + 1] = static_cast<uint8_t>((s >> 8) & 0xFF);
    }

    return data;
}

inline void toModalJson(nlohmann::json& j, const ModalMode& m)
{
    j = nlohmann::json
    {
        {"frequency", m.frequency},
        {"amplitude", m.amplitude},
        {"decayTime", m.decayTime},
        {"phase", m.phase},
        {"startSec", m.startSec},
        {"gain", m.gain},
        {"Bandwidth", m.bandwidth},
        {"Inharmonicity", m.inharmonicity},
        {"Noise Mix", m.noiseMix},
        {"Harmonic Mask", m.harmonicMask},
        {"Random Phase", m.randomPhase},
        {"Random Decay", m.randomDecay},
        {"Clip Amount", m.clipAmount}
    };
}

inline void fromModalJson(nlohmann::json& j, ModalMode& m)
{
    m.frequency = j.value("frequency", 0.0f);
    m.amplitude = j.value("amplitude", 0.0f);
    m.decayTime = j.value("decayTime", 0.0f);
    m.phase = j.value("phase", 0.0f);
    m.startSec = j.value("startSec", 0.0f);
    m.gain = j.value("gain", 1.0f);

    m.bandwidth = j.value("Bandwidth", 0.0f);
    m.inharmonicity = j.value("Inharmonicity", 0.0f);
    m.noiseMix = j.value("Noise Mix", 0.0f);
    m.harmonicMask = j.value("Harmonic Mask", 0.0f);
    m.randomPhase = j.value("Random Phase", 0.0f);
    m.randomDecay = j.value("Random Decay", 0.0f);
    m.clipAmount = j.value("Clip Amount", 0.0f);
    m.lowpassCutoff = j.value("lowpassCutoff", 20.0f);
    m.highpassCutoff = j.value("highpassCutoff", 20000.0f);
}

inline static std::vector<ModalMode> loadModalDataJson(const char* filePath)
{
    std::ifstream ifs(filePath);
    if (!ifs.is_open()) {
        std::cerr << "Failed to open preset file: " << filePath << std::endl;
    }

    nlohmann::json j;

    try {
        ifs >> j;
    }
    catch (...) {
        std::cerr << "JSON parse error:" << filePath << std::endl;
    }

    if (!j.contains("modals") || !j["modals"].is_array()) {
        std::cerr << "No 'modals' array in preset JSON:" << filePath << std::endl;
    }

    std::vector<ModalMode> modals;
    modals.reserve(j["modals"].size());
    for (auto& jm : j["modals"]) {
        ModalMode m;
        fromModalJson(jm, m);
        modals.push_back(m);
    }
    return modals;
}

//inline std::vector<int16_t>convertData8to16(const std::vector<uint8_t>& data)
//{
//    //端数切捨て
//    size_t byteCount = data.size() & ~static_cast<size_t>(1);
//    size_t samplesCount = byteCount / 2;
//
//    std::vector<int16_t> samples;
//    samples.resize(samplesCount);
//
//    for (size_t i = 0; i < samplesCount; ++i)
//    {
//        uint16_t lo = static_cast<uint8_t>(data[i * 2]);
//        uint16_t hi = static_cast<uint8_t>(data[i * 2 + 1]);
//        uint16_t u = static_cast<uint16_t>(lo | (hi << 8));
//        samples[i] = static_cast<int16_t>(u);
//    }
//    return samples;
//}
