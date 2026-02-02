#include"DSPMath.h"

namespace DSP
{
    void Avx2SClampedF32To16(uint8_t*dst, __m256 scaled)
    {
        // float → int32
        __m256i i32 = _mm256_cvtps_epi32(clampPCM(scaled));

        // int32 → int16（パック）
        __m128i packed = _mm_packs_epi32( 
            _mm256_castsi256_si128(i32), //lo
            _mm256_extracti128_si256(i32, 1) //hi
        );

        // LE 書き込み
        _mm_storeu_si128(reinterpret_cast<__m128i*>(dst), packed);
    }
}