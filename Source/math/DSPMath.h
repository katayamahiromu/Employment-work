#pragma once
#include <cstdint>
#include <immintrin.h>
#include<DirectXMath.h>

/// <summary>
/// AVX2で使うDSP用の自作計算置き場
/// </summary>
namespace DSP
{
	using Sample8F = __m256;
	using Sample4D = __m256d;

	//定数
	alignas(32) inline const Sample8F scale8 = _mm256_set1_ps(30000.0f);
	alignas(32) inline const Sample8F max8 = _mm256_set1_ps(32760.0f);
	alignas(32) inline const Sample8F min8 = _mm256_set1_ps(-32760.0f);
	alignas(32) inline const Sample8F idx8 = _mm256_set_ps(7, 6, 5, 4, 3, 2, 1, 0);

	alignas(32) inline const Sample8F half = _mm256_set1_ps(0.5f);
	alignas(32) inline const Sample8F two = _mm256_set1_ps(2.0f);
	alignas(32) inline const Sample8F one = _mm256_set1_ps(1.0f);
	alignas(32) inline const Sample8F four = _mm256_set1_ps(4.0f);
	alignas(32) inline const Sample8F nOne = _mm256_set1_ps(-1.0f);

	alignas(32) inline const Sample8F twoPI = _mm256_set1_ps(DirectX::XM_2PI);
	alignas(32) inline const Sample8F eight = _mm256_set1_ps(8.0f);


	//double SIMD 用 idx（7〜0 を 4 要素×2 に分割）
	alignas(32) inline const Sample4D idx4d_hi = _mm256_set_pd(7.0, 6.0, 5.0, 4.0);
	alignas(32) inline const Sample4D idx4d_lo = _mm256_set_pd(3.0, 2.0, 1.0, 0.0);
	alignas(32) inline const Sample4D one4d = _mm256_set1_pd(1.0);
	alignas(32) inline const Sample4D eightD = _mm256_set1_pd(8.0f);

	struct alignas(32) XorShift128Plus8
	{
		__m256i s0;
		__m256i s1;
	};

	struct alignas(32) XorShift32x8
	{
		__m256i state;
	};

	//関数
	void Avx2SClampedF32To16(uint8_t* dst,__m256 scaled);

	inline Sample8F loadFloat(float num) { return _mm256_set1_ps(num); }
	inline Sample8F loadFloat8(const uint8_t* src) 
	{
		// 1) 8 バイトをロード（128bit）
		__m128i bytes = _mm_loadl_epi64((const __m128i*)src);

		// 2) 8bit → 16bit に拡張
		__m128i u16 = _mm_cvtepu8_epi16(bytes);

		// 3) 下位 8 個を float に変換
		__m256 f32 = _mm256_cvtepi32_ps(_mm256_cvtepu16_epi32(u16));

		return f32;
	}

	inline __m256i xs32_next(XorShift32x8& st)
	{
		__m256i x = st.state;

		x = _mm256_xor_si256(x, _mm256_slli_epi32(x, 13));
		x = _mm256_xor_si256(x, _mm256_srli_epi32(x, 17));
		x = _mm256_xor_si256(x, _mm256_slli_epi32(x, 5));

		st.state = x;
		return x;
	}


	inline __m256i xs128p_next(XorShift128Plus8& st)
	{
		__m256i s1 = st.s0;
		const __m256i s0 = st.s1;

		st.s0 = s0;

		s1 = _mm256_xor_si256(s1, _mm256_slli_epi64(s1, 23));
		__m256i t = _mm256_xor_si256(s1, s0);
		t = _mm256_xor_si256(t, _mm256_srli_epi64(s1, 17));
		t = _mm256_xor_si256(t, _mm256_srli_epi64(s0, 26));

		st.s1 = t;

		return _mm256_add_epi64(t, s0);
	}

	//絶対値
	__forceinline Sample8F abs(Sample8F x){return _mm256_and_ps(x, _mm256_castsi256_ps(_mm256_set1_epi32(0x7FFFFFFF)));}

	//位相量
	__forceinline Sample8F PhaseAmount(float w){ return _mm256_mul_ps(idx8, _mm256_set1_ps(w)); }

	//PCMの範囲でクランプ
	__forceinline Sample8F clampPCM(Sample8F scaled) { return _mm256_min_ps(max8, _mm256_max_ps(min8, scaled)); }

	//位相を進める
	__forceinline Sample8F advancePhase(Sample8F phase, Sample8F inc)
	{
		phase = _mm256_add_ps(phase, inc);
		return _mm256_sub_ps(phase, _mm256_floor_ps(phase));
	}

	//SIMDでの範囲ラップ
	__forceinline Sample8F WrapPhaseGE(Sample8F phase, Sample8F wrapValue)
	{
		return _mm256_sub_ps(
			phase,
			_mm256_and_ps(
				_mm256_cmp_ps(phase, wrapValue, _CMP_GE_OQ),
				wrapValue
			)
		);
	}

	//サイン波生成
	__forceinline Sample8F SinWave(Sample8F phase,Sample8F phaseInc)
	{
		// phase8 = phase_base + idx8 * w
		Sample8F phase8 = _mm256_fmadd_ps(idx8, phaseInc, phase);
		return _mm256_mul_ps(_mm256_sin_ps(phase8), scale8);
	}

	//ノコギリ波生成
	__forceinline Sample8F SawWave(__m256d base4d, __m256d inc)
	{

		// --- 前半4 ---
		__m256d phase4d_hi = _mm256_fmadd_pd(idx4d_hi, inc, base4d);
		__m256d mask_hi = _mm256_cmp_pd(phase4d_hi, one4d, _CMP_GE_OQ);
		phase4d_hi = _mm256_sub_pd(phase4d_hi, _mm256_and_pd(mask_hi, one4d));
		__m128 phase4f_hi = _mm256_cvtpd_ps(phase4d_hi);

		// --- 後半4 ---
		__m256d phase4d_lo = _mm256_fmadd_pd(idx4d_lo, inc, base4d);
		__m256d mask_lo = _mm256_cmp_pd(phase4d_lo, one4d, _CMP_GE_OQ);
		phase4d_lo = _mm256_sub_pd(phase4d_lo, _mm256_and_pd(mask_lo, one4d));
		__m128 phase4f_lo = _mm256_cvtpd_ps(phase4d_lo);

		// --- 8本に結合 ---
		__m256 phase8 = _mm256_castps128_ps256(phase4f_lo);
		phase8 = _mm256_insertf128_ps(phase8, phase4f_hi, 1);

		Sample8F saw = _mm256_sub_ps(_mm256_mul_ps(phase8, two), one);
		return clampPCM(_mm256_mul_ps(saw, scale8));
	}

	//矩形波
	__forceinline Sample8F square(Sample8F phase)
	{
		Sample8F mask = _mm256_cmp_ps(phase, half, _CMP_LT_OS);
		return _mm256_blendv_ps(nOne, one, mask);
	}

	//三角波生成
	__forceinline DSP::Sample8F TriangleWave(Sample8F phase)
	{
		// tri = 2 * fabs(2*(phase - floor(phase + 0.5))) - 1

		Sample8F t = _mm256_floor_ps(_mm256_add_ps(phase, half));
		Sample8F v = _mm256_sub_ps(phase, t);
		v = _mm256_mul_ps(v, two);
		v = DSP::abs(v);
		v = _mm256_mul_ps(v, two);
		v = _mm256_sub_ps(v, one);

		// PCM スケール
		return _mm256_mul_ps(v, DSP::scale8);
	}


	//データの書き出し
	__forceinline void StorePCMData(uint8_t* &dst, Sample8F wave,int buf)
	{
		Avx2SClampedF32To16(dst, wave);
		dst += buf;
	}

	//値を0~2πの間で抑え込む
	__forceinline Sample8F UpdatePhase2PI(Sample8F phase, Sample8F inc)
	{
		// 8 サンプル分進める：inc = phase * 8
		return WrapPhaseGE(_mm256_fmadd_ps(inc, eight, phase), twoPI);
	}

	//値を0~1で抑え込む
	__forceinline DSP::Sample8F UpdatePhase(DSP::Sample8F phase, DSP::Sample8F inc)
	{
		// 0..1 wrap
		return DSP::WrapPhaseGE(_mm256_add_ps(phase, inc), DSP::one);
	}


	//double型で0~1で抑え込む
	__forceinline Sample4D UpdatePhase4d(__m256d base4d, __m256d inc4d)
	{
		__m256d next = _mm256_fmadd_pd(inc4d, eightD, base4d);

		__m256d mask = _mm256_cmp_pd(next, DSP::one4d, _CMP_GE_OQ);
		return _mm256_sub_pd(next, _mm256_and_pd(mask, DSP::one4d));
	}

	__forceinline Sample8F Lowpass(Sample8F x, float a, float& prev)
	{
		const __m256 a8 = _mm256_set1_ps(a);
		const __m256 one_minus_a8 = _mm256_set1_ps(1.0f - a);

		// y[0] = a*x[0] + (1-a)*prev
		alignas(32) float in[8];
		_mm256_store_ps(in, x);

		float y[8];
		y[0] = a * in[0] + (1.0f - a) * prev;

		// y[i] = a*x[i] + (1-a)*y[i-1]
		for (int i = 1; i < 8; ++i)
			y[i] = a * in[i] + (1.0f - a) * y[i - 1];

		prev = y[7];

		return _mm256_load_ps(y);
	}
}