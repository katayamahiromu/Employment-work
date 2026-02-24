#include"SignalMixer.h"
#include"SoundHealper.h"
#include<math.h>
#include<algorithm>
#include<random>
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
    const double sr = static_cast<double>(SamplingRate);

    size_t csize = carrier.samples.size();
    size_t msize = mod.samples.size();
    size_t numSamples = csize;
    if (numSamples == 0) return std::vector<UINT8>{};

    std::vector<int> buf(numSamples, 0);

    double phaseC = carrier.phase;
    double baseFreq = carrier.frequency;
    // 基本の位相増分（ラジアン/サンプル）
    double baseDphi = (baseFreq > 0.0) ? DirectX::XM_2PI * baseFreq / sr : 0.0;

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
        double instDphi = baseDphi + DirectX::XM_2PI * (static_cast<double>(modulationDepth) * modValue) / sr;

        // 位相更新（ここでモジュレーションが周波数として反映される）
        phaseC += instDphi;

        // 位相ラップ（while で確実に処理）
        while (phaseC >= DirectX::XM_2PI) phaseC -= DirectX::XM_2PI;
        while (phaseC < 0.0) phaseC += DirectX::XM_2PI;

        // 出力（振幅と全体ゲイン）
        double sample = sin(phaseC) * 32767.0 * static_cast<double>(carrier.gain) * static_cast<double>(gain);

        // クリップ
        if (sample > 32767.0) sample = 32767.0;
        if (sample < -32768.0) sample = -32768.0;

        buf[n] = static_cast<int>(lround(sample));
    }

    return EncodePCM16LE(buf);
}

std::vector<UINT8> SignalMixer::frequencyModulation(waveData& carrierData, float modulationDepth, float gain)
{
    const double sr = static_cast<double>(SamplingRate);

    size_t numSamples = carrierData.samples.size();
    if (numSamples == 0) return {};

    std::vector<int> buf(numSamples);

    double phase = 0.0;

    for (size_t n = 0; n < numSamples; ++n)
    {
        // 16bit 読み出し
        int16_t s = static_cast<int16_t>(carrierData.samples[n]);
        double x = static_cast<double>(s) / 32768.0;  // -1.0〜1.0

        //自己FM（物理型）
        phase += modulationDepth * x;

        // 波形生成
        double y = sin(phase);

        // ゲイン
        y *= gain;

        // クリップ
        y = std::clamp(y, -1.0, 1.0);

        buf[n] = static_cast<int>(lround(y * 32767.0));
    }

    return EncodePCM16LE(buf);
}

std::vector<UINT8> SignalMixer::amplitudeModulation(waveData& carrierData, waveData& modData, float modulationDepth, float gain)
{
    const double sr = static_cast<double>(SamplingRate);

    size_t csize = carrierData.samples.size();
    size_t msize = modData.samples.size();
    size_t numSamples = csize;
    if (numSamples == 0) return std::vector<UINT8>{};

    std::vector<int> buf(numSamples, 0);

    double phaseC = carrierData.phase;
    double dphiC = (carrierData.frequency > 0.0) ? DirectX::XM_2PI * static_cast<double>(carrierData.frequency) / sr : 0.0;

    // モジュレータ位相と増分（modData.frequency を利用）
    double phaseM = modData.phase;
    double dphiM = (msize > 0 && modData.frequency > 0.0f) ? DirectX::XM_2PI * static_cast<double>(modData.frequency) / sr : 0.0;

    for (size_t n = 0; n < numSamples; ++n)
    {
        // --- mod を位相で読み出し（サブサンプル補間） ---
        double m = 0.0;
        if (msize > 0)
        {
            // modPhase を 0..1 に正規化してインデックス化
            double pos = (phaseM / DirectX::XM_2PI) * static_cast<double>(msize);
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
        while (phaseC >= DirectX::XM_2PI) phaseC -= DirectX::XM_2PI;
        while (phaseC < 0.0) phaseC += DirectX::XM_2PI;

        // モジュレータ位相更新（次サンプルへ）
        phaseM += dphiM;
        while (phaseM >= DirectX::XM_2PI) phaseM -= DirectX::XM_2PI;
        while (phaseM < 0.0) phaseM += DirectX::XM_2PI;

        double sample = sin(phaseC) * 32767.0 * static_cast<double>(carrierData.gain) * static_cast<double>(gain) * amplitude;

        if (sample > 32767.0) sample = 32767.0;
        if (sample < -32768.0) sample = -32768.0;
        buf[n] = static_cast<int>(lround(sample));
    }

    return EncodePCM16LE(buf);
}

std::vector<UINT8> SignalMixer::amplitudeModulation(waveData& carrierData, float modulationDepth, float lfoFreq, float gain)
{
    const double sr = static_cast<double>(SamplingRate);

    size_t numSamples = carrierData.samples.size();
    if (numSamples == 0) return {};

    std::vector<int> buf(numSamples);

    // LFO 用
    double phase = 0.0;
    double dphi = DirectX::XM_2PI * static_cast<double>(lfoFreq) / sr;

    for (size_t n = 0; n < numSamples; ++n)
    {
        //元の音を読み出し
        int16_t s = static_cast<int16_t>(carrierData.samples[n]);
        double x = static_cast<double>(s) / 32768.0;

        // LFO で AM 包絡を作る
        double lfo = 0.5 * (sin(phase) + 1.0); // 0〜1
        double env = (1.0 - modulationDepth) + modulationDepth * lfo;

        //AM 適用
        double y = x * env * gain;

        //クリップ
        y = std::clamp(y, -1.0, 1.0);

        buf[n] = static_cast<int>(lround(y * 32767.0));

        //LFO 位相更新
        phase += dphi;
        if (phase >= DirectX::XM_2PI) phase -= DirectX::XM_2PI;
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
        if (phaceC > DirectX::XM_2PI) phaceC -= DirectX::XM_2PI;
        if (phaceM > DirectX::XM_2PI) phaceM -= DirectX::XM_2PI;
    }

    return EncodePCM16LE(buf);
}

std::vector<UINT8> SignalMixer::vibratoLFO(waveData& carrierData, float semitones, float lfoFreq, float gain)
{
    const double sr = static_cast<double>(SamplingRate);

    size_t numSamples = carrierData.samples.size();
    if (numSamples == 0) return {};

    std::vector<int> buf(numSamples);

    // LFO 用
    double lfoPhase = 0.0;
    double lfoDphi = DirectX::XM_2PI * static_cast<double>(lfoFreq) / sr;

    // 読み出し位置（サンプル単位の浮動小数点インデックス）
    double readIndex = static_cast<double>(carrierData.phase); // carrierData.phase を初期サンプル位置として利用（0 なら先頭）
    if (readIndex < 0.0) readIndex = 0.0;
    if (readIndex >= static_cast<double>(numSamples)) readIndex = 0.0;

    for (size_t n = 0; n < numSamples; ++n)
    {
        // LFO から瞬時のピッチ変動（-1..1）を得る
        double lfo = sin(lfoPhase); // -1 .. 1

        // 半音単位での変動量（-modulationDepth .. +modulationDepth）
        double semitone = lfo * static_cast<double>(semitones);

        // ピッチ比（再生レート倍率）を計算: 2^(semitone/12)
        double rate = pow(2.0, semitone / 12.0);

        // 現在の読み出し位置から線形補間でサンプルを取得
        // インデックスの整数部と小数部
        size_t idx0 = static_cast<size_t>(floor(readIndex));
        size_t idx1 = idx0 + 1;
        if (idx1 >= numSamples) idx1 = numSamples - 1;

        double frac = readIndex - static_cast<double>(idx0);

        // PCM を -1.0 .. +1.0 に正規化
        double s0 = static_cast<double>(carrierData.samples[idx0]) / 32768.0;
        double s1 = static_cast<double>(carrierData.samples[idx1]) / 32768.0;

        double x = s0 + (s1 - s0) * frac;

        // ゲイン適用
        double y = x * static_cast<double>(gain);

        // クリップ
        y = std::clamp(y, -1.0, 1.0);

        buf[n] = static_cast<int>(lround(y * 32767.0));

        // 読み出し位置を更新（可変レート）
        readIndex += rate;

        // 範囲を超えたらループ（単純に折り返すか停止させたい場合はここを変更）
        if (readIndex >= static_cast<double>(numSamples - 1))
        {
            // ループ再生する場合は先頭に戻す
            readIndex -= static_cast<double>(numSamples);
            if (readIndex < 0.0) readIndex = 0.0;
        }

        // LFO 位相更新
        lfoPhase += lfoDphi;
        if (lfoPhase >= DirectX::XM_2PI) lfoPhase -= DirectX::XM_2PI;
    }

    return EncodePCM16LE(buf);
}

std::vector<UINT8> SignalMixer::granularize(
    waveData& src,
    float grainMs,        // グレイン長（ms）
    float densityHz,      // 1秒あたりのグレイン数（密度）
    float pitchRandSemis, // ピッチランダム（±半音）
    float posJitterMs,    // 出力配置ジッター（ms）
    float gain)           // 出力ゲイン
{
    const double sr = static_cast<double>(SamplingRate);
    size_t srcN = src.samples.size();
    if (srcN == 0) return {};

    int grainSamples = max(1, static_cast<int>(std::round(grainMs * 0.001 * sr)));
    double seconds = static_cast<double>(srcN) / sr;
    size_t outN = srcN;

    // 出力バッファ（doubleでミックス）
    std::vector<double> out(outN, 0.0);

    // ハニング窓（グレイン長）
    std::vector<double> window(grainSamples);
    for (int i = 0; i < grainSamples; ++i) {
        window[i] = 0.5 * (1.0 - cos(DirectX::XM_2PI * i / (grainSamples - 1)));
    }

    // PRNG（状態はここで保持）
    std::mt19937_64 rng((uint64_t)123456789); // 実運用は可変シード
    std::uniform_real_distribution<double> uni01(0.0, 1.0);
    std::uniform_real_distribution<double> uniNeg1(-1.0, 1.0);

    // hop（サンプル）: densityHz に基づく平均間隔
    double avgHop = sr / static_cast<double>(max(1.0f, densityHz));
    // posJitter をサンプルに変換
    int jitterSamples = static_cast<int>(std::round(posJitterMs * 0.001 * sr));

    // 線形補間関数（ラムダ）
    auto sampleAt = [&](double idx)->double {
        // ラップさせることで範囲外の不連続を避ける（必要に応じて変更）
        if (srcN == 0) return 0.0;
        // wrap
        while (idx < 0.0) idx += static_cast<double>(srcN);
        while (idx >= static_cast<double>(srcN - 1)) idx -= static_cast<double>(srcN - 1);
        size_t i0 = static_cast<size_t>(floor(idx));
        size_t i1 = i0 + 1;
        if (i1 >= srcN) i1 = srcN - 1;
        double frac = idx - static_cast<double>(i0);
        double s0 = static_cast<double>(src.samples[i0]) / 32768.0;
        double s1 = static_cast<double>(src.samples[i1]) / 32768.0;
        return s0 + (s1 - s0) * frac;
        };

    // 出力上にグレインを順次配置する
    // startPos を 0 から seconds に沿って進める（平均 hop）
    double currentOutPos = 0.0;
    while (currentOutPos < static_cast<double>(outN)) {
        // ジッター（± jitterSamples）
        int jitter = (jitterSamples > 0) ? static_cast<int>(std::round((uniNeg1(rng) * jitterSamples))) : 0;
        int outStart = static_cast<int>(std::round(currentOutPos)) + jitter;
        if (outStart < 0) outStart = 0;
        if (outStart >= static_cast<int>(outN)) break;

        // src の読み出し開始位置（ランダムに散らす）
        double srcBaseSec = uni01(rng) * seconds;
        double srcStartIdx = srcBaseSec * sr;

        // ピッチランダム（±pitchRandSemis）
        double semis = pitchRandSemis * uniNeg1(rng);
        double rate = pow(2.0, semis / 12.0);

        // 振幅ランダム（小さめ）
        double ampRand = 0.8 + 0.4 * uni01(rng); // 0.8..1.2

        // グレインを重ねる（窓＋線形補間）
        for (int i = 0; i < grainSamples; ++i) {
            int outIdx = outStart + i;
            if (outIdx < 0) continue;
            if (outIdx >= static_cast<int>(outN)) break;
            double srcIdx = srcStartIdx + i * rate;
            double s = sampleAt(srcIdx);
            double w = window[i];
            // optional: per-grain envelope shaping (attack/decay) already in window
            out[outIdx] += s * w * ampRand;
        }

        // 次のグレイン開始位置を hop 分進める（平均）
        currentOutPos += avgHop;
    }

    // 正規化とゲイン、クリップして int バッファへ
    std::vector<int> intBuf(outN);
    double maxAbs = 1e-12;
    for (size_t i = 0; i < outN; ++i) maxAbs = max(maxAbs, std::abs(out[i]));
    double norm = (maxAbs > 1.0) ? (1.0 / maxAbs) : 1.0;
    for (size_t i = 0; i < outN; ++i) {
        double y = out[i] * norm * static_cast<double>(gain);
        y = std::clamp(y, -1.0, 1.0);
        intBuf[i] = static_cast<int>(lround(y * 32767.0));
    }

    return EncodePCM16LE(intBuf);
}