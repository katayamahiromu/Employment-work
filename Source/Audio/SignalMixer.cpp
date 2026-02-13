#include"SignalMixer.h"
#include"SoundHealper.h"
#include<math.h>
#include<algorithm>
#include<DirectXMath.h>

std::vector<UINT8> SignalMixer::mix(std::vector<waveData>& waveArray)
{
    //配列がからのときは空のバッファを返す
    if (waveArray.empty())return std::vector<UINT8>{};
 
    //サンプル数の取得
    size_t maxSamples = 0;
    for (const auto& w : waveArray) {
        maxSamples = max(maxSamples, w.samples.size());
    }

    //サンプル数が0の時にリターン
    if (maxSamples == 0) return std::vector<UINT8>{};

    // 合成用バッファ
    std::vector<int> buf(maxSamples, 0);

    // 加算合成（gain を掛ける）
    for (const auto& w : waveArray) {
        float g = w.gain;
        for (size_t i = 0; i < w.samples.size(); ++i) {
            buf[i] += static_cast<int>(lround(static_cast<float>(w.samples[i]) * g));
        }
    }

    return EncodePCM16LE(buf);
}

std::vector<UINT8> SignalMixer::frequencyModulation(waveData& carrier, waveData& mod, float modulationDepth, float gain)
{
    const double twoPi = 2.0 * DirectX::XM_PI;
    const double sr = static_cast<double>(SamplingRate);

    size_t csize = carrier.samples.size();
    size_t msize = mod.samples.size();
    size_t numSamples = csize;
    if (numSamples == 0) return std::vector<UINT8>{};

    std::vector<int> buf(numSamples, 0);

    double phaseC = carrier.phase;
    double baseFreq = carrier.frequency;
    // 基本の位相増分（ラジアン/サンプル）
    double baseDphi = (baseFreq > 0.0) ? twoPi * baseFreq / sr : 0.0;

    for (size_t n = 0; n < numSamples; ++n)
    {
        // モジュレータをループ（必要なら補間に置き換える）
        size_t midx = (msize > 0) ? (n % msize) : 0;

        // mod.samples を int16_t と仮定して -1..+1 に正規化
        double modValue = 0.0;
        if (msize > 0) {
            int16_t mS = static_cast<int16_t>(mod.samples[midx]);
            modValue = static_cast<double>(mS) / 32768.0; // -1..+1
            modValue *= mod.gain;
        }

        // modulationDepth を Hz 単位と想定。瞬時位相増分に変換して加算
        double instDphi = baseDphi + twoPi * (static_cast<double>(modulationDepth) * modValue) / sr;

        // 位相更新（ここでモジュレーションが周波数として反映される）
        phaseC += instDphi;

        // 位相ラップ（while で確実に処理）
        while (phaseC >= twoPi) phaseC -= twoPi;
        while (phaseC < 0.0) phaseC += twoPi;

        // 出力（振幅と全体ゲイン）
        double sample = sin(phaseC) * 32767.0 * static_cast<double>(carrier.gain) * static_cast<double>(gain);

        // クリップ
        if (sample > 32767.0) sample = 32767.0;
        if (sample < -32768.0) sample = -32768.0;

        buf[n] = static_cast<int>(lround(sample));
    }

    return EncodePCM16LE(buf);
}

std::vector<UINT8> SignalMixer::amplitudeModulation(waveData& carrierData, waveData& modData, float modulationDepth, float gain)
{
    const double twoPi = 2.0 * DirectX::XM_PI;
    const double sr = static_cast<double>(SamplingRate);

    size_t csize = carrierData.samples.size();
    size_t msize = modData.samples.size();
    size_t numSamples = csize;
    if (numSamples == 0) return std::vector<UINT8>{};

    std::vector<int> buf(numSamples, 0);

    double phaseC = carrierData.phase;
    double dphiC = (carrierData.frequency > 0.0) ? twoPi * static_cast<double>(carrierData.frequency) / sr : 0.0;

    // モジュレータ位相と増分（modData.frequency を利用）
    double phaseM = modData.phase;
    double dphiM = (msize > 0 && modData.frequency > 0.0f) ? twoPi * static_cast<double>(modData.frequency) / sr : 0.0;

    for (size_t n = 0; n < numSamples; ++n)
    {
        // --- mod を位相で読み出し（サブサンプル補間） ---
        double m = 0.0;
        if (msize > 0)
        {
            // modPhase を 0..1 に正規化してインデックス化
            double pos = (phaseM / twoPi) * static_cast<double>(msize);
            // floor と frac
            size_t i0 = static_cast<size_t>(floor(pos)) % msize;
            size_t i1 = (i0 + 1) % msize;
            double frac = pos - floor(pos);
            // サンプルは int16_t と仮定
            int16_t s0 = static_cast<int16_t>(modData.samples[i0]);
            int16_t s1 = static_cast<int16_t>(modData.samples[i1]);
            double v0 = static_cast<double>(s0) / 32768.0;
            double v1 = static_cast<double>(s1) / 32768.0;
            m = v0 + (v1 - v0) * frac; // 線形補間
            m *= modData.gain;
        }

        // -1..+1 -> 0..1 の包絡にし、depth でブレンド（負の振幅を避ける）
        double env = (m + 1.0) * 0.5;
        double amplitude = (1.0 - static_cast<double>(modulationDepth)) + static_cast<double>(modulationDepth) * env;

        // キャリア位相更新
        phaseC += dphiC;
        while (phaseC >= twoPi) phaseC -= twoPi;
        while (phaseC < 0.0) phaseC += twoPi;

        // モジュレータ位相更新（次サンプルへ）
        phaseM += dphiM;
        while (phaseM >= twoPi) phaseM -= twoPi;
        while (phaseM < 0.0) phaseM += twoPi;

        double sample = sin(phaseC) * 32767.0 * static_cast<double>(carrierData.gain) * static_cast<double>(gain) * amplitude;

        if (sample > 32767.0) sample = 32767.0;
        if (sample < -32768.0) sample = -32768.0;
        buf[n] = static_cast<int>(lround(sample));
    }

    return EncodePCM16LE(buf);
}

std::vector<UINT8> SignalMixer::ringModulation(waveData& carrierData, waveData& modData, float modulationDepth, float gain)
{
    float carrierFreq = carrierData.frequency;
    float modFreq = modData.frequency;

    // 出力長は最大長に合わせる（短い方はループ参照）
    size_t csize = carrierData.samples.size();
    size_t msize = modData.samples.size();
    size_t numSamples = csize;
    if (numSamples == 0)  return std::vector<UINT8>{};

    // 合成用バッファ
    std::vector<int> buf(numSamples, 0);

    //キャリア位相と基本位相増分
    double phaceC = carrierData.phase;
    double dphiC = (carrierFreq > 0.0f) ? DirectX::XM_2PI * static_cast<double>(carrierFreq) / SamplingRate : 0.0;

    //モジュレータ位相
    double phaceM = modData.phase;
    double dphiM = (modFreq > 0.0f) ? DirectX::XM_2PI * static_cast<double>(modFreq) / SamplingRate : 0.0;

    //ループでモジュレータを使用
    for (size_t n = 0;n < numSamples;++n)
    {
        // モジュレータ値 (-1.0〜+1.0)
        double modValue = 0.0;
        if (msize > 0) {
            size_t idx = n % msize;
            modValue = (static_cast<double>(modData.samples[idx]) / 32767.0) * modData.gain;
        }

        //振幅を変調
        phaceC += dphiC;
        phaceM += dphiM;

        double carrierSample = sin(phaceC) * modValue;
        double modeSample = sin(phaceM);

        double sample = carrierSample * modeSample * 32767.0 * carrierData.gain * gain;

        buf[n] = static_cast<int>(round(sample));

        // 位相ラップ
        if (phaceC > 2.0 * DirectX::XM_PI) phaceC -= 2.0 * DirectX::XM_PI;
        if (phaceM > 2.0 * DirectX::XM_PI) phaceM -= 2.0 * DirectX::XM_PI;
    }

    return EncodePCM16LE(buf);
}