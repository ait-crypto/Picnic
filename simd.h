/*
 *  This file is part of the optimized implementation of the Picnic signature scheme.
 *  See the accompanying documentation for complete details.
 *
 *  The code is provided under the MIT license, see LICENSE for
 *  more details.
 *  SPDX-License-Identifier: MIT
 */

#ifndef SIMD_H
#define SIMD_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "macros.h"

#if defined(_MSC_VER)
#include <immintrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
#include <x86intrin.h>
#elif defined(__GNUC__) && defined(__ARM_NEON)
#include <arm_neon.h>
#endif

#include "cpu.h"

#if defined(__x86_64__) || defined(_M_X64)
#if defined(WITH_SSE2) || defined(WITH_AVX2)
#define NO_UINT64_FALLBACK
#endif
#endif

#if defined(__aarch64__)
#if defined(WITH_NEON)
#define NO_UINT64_FALLBACK
#endif
#endif

#if defined(_MSC_VER)
#define restrict __restrict
#endif

#if defined(WITH_SSE2)
#define ATTR_TARGET_S128 ATTR_TARGET_SSE2
#else
#define ATTR_TARGET_S128
#endif

#if defined(WITH_AVX2)
#define ATTR_TARGET_S256 ATTR_TARGET_AVX2
#else
#define ATTR_TARGET_S256
#endif

#define FN_ATTRIBUTES_S256 ATTR_ARTIFICIAL ATTR_ALWAYS_INLINE ATTR_TARGET_S256
#define FN_ATTRIBUTES_S128 ATTR_ARTIFICIAL ATTR_ALWAYS_INLINE ATTR_TARGET_S128

#define FN_ATTRIBUTES_S256_PURE FN_ATTRIBUTES_S256 ATTR_PURE
#define FN_ATTRIBUTES_S128_PURE FN_ATTRIBUTES_S128 ATTR_PURE

#define FN_ATTRIBUTES_S256_CONST FN_ATTRIBUTES_S256 ATTR_CONST
#define FN_ATTRIBUTES_S128_CONST FN_ATTRIBUTES_S128 ATTR_CONST

#if defined(WITH_SSE2)
/* backwards compatibility macros for GCC 4.8 and 4.9
 *
 * bs{l,r}i was introduced in GCC 5 and in clang as macros sometime in 2015.
 * */
#if (!defined(__clang__) && defined(__GNUC__) && __GNUC__ < 5) ||                                  \
    ((defined(__clang__) || defined(_MSC_VER)) && !defined(_mm_bslli_si128))
#define _mm_bslli_si128(a, imm) _mm_slli_si128((a), (imm))
#define _mm_bsrli_si128(a, imm) _mm_srli_si128((a), (imm))
#endif
#endif

#define apply_region(name, type, op, attributes)                                                   \
  static inline void attributes name(type* restrict dst, type const* restrict src,                 \
                                     unsigned int count) {                                         \
    for (unsigned int i = count; i; --i, ++dst, ++src) {                                           \
      *dst = op(*dst, *src);                                                                       \
    }                                                                                              \
  }

#define apply_mask(name, type, op, opmask, attributes)                                             \
  static inline type attributes name(const type lhs, const type rhs, const type mask) {            \
    return op(lhs, opmask(rhs, mask));                                                             \
  }

#define apply_mask_region(name, type, load, op, opmask, attributes)                                \
  static inline void attributes name(type* restrict dst, const uint64_t* restrict src,             \
                                     const type mask, unsigned int count) {                        \
    for (unsigned int i = count; i; --i, ++dst, src += sizeof(type) / sizeof(uint64_t)) {          \
      *dst = op(*dst, opmask(load(src), mask));                                                    \
    }                                                                                              \
  }

#define apply_array(name, type, op, count, attributes)                                             \
  static inline void attributes name(type dst[count], type const lhs[count],                       \
                                     type const rhs[count]) {                                      \
    type* d       = dst;                                                                           \
    const type* l = lhs;                                                                           \
    const type* r = rhs;                                                                           \
    for (unsigned int i = count; i; --i, ++d, ++l, ++r) {                                          \
      *d = op(*l, *r);                                                                             \
    }                                                                                              \
  }

#if defined(WITH_AVX2)
typedef __m256i word256;

#if defined(__GNUC__) || defined(__clang__)
#define _mm256_set_m128i(v0, v1) _mm256_insertf128_si256(_mm256_castsi128_si256(v1), (v0), 1)
#define _mm256_setr_m128i(v0, v1) _mm256_set_m128i((v1), (v0))
#endif

#define mm256_zero _mm256_setzero_si256()
#define mm256_load(s) _mm256_load_si256((const word256*)s)
#define mm256_set_4(v0, v1, v2, v3) _mm256_set_epi64x(v3, v2, v1, v0)
#define mm256_store(d, s) _mm256_store_si256((word256*)d, s)
#define mm256_xor(l, r) _mm256_xor_si256((l), (r))
#define mm256_and(l, r) _mm256_and_si256((l), (r))

// clang-format off
apply_mask(mm256_xor_mask, word256, mm256_xor, mm256_and, FN_ATTRIBUTES_S256_CONST)
// clang-format on

#define mm256_shift_left(data, count)                                                              \
  _mm256_or_si256(_mm256_slli_epi64(data, count),                                                  \
                  _mm256_blend_epi32(mm256_zero,                                                   \
                                     _mm256_permute4x64_epi64(_mm256_srli_epi64(data, 64 - count), \
                                                              _MM_SHUFFLE(2, 1, 0, 0)),            \
                                     _MM_SHUFFLE(3, 3, 3, 0)))

#define mm256_shift_right(data, count)                                                             \
  _mm256_or_si256(_mm256_srli_epi64(data, count),                                                  \
                  _mm256_blend_epi32(mm256_zero,                                                   \
                                     _mm256_permute4x64_epi64(_mm256_slli_epi64(data, 64 - count), \
                                                              _MM_SHUFFLE(0, 3, 2, 1)),            \
                                     _MM_SHUFFLE(0, 3, 3, 3)))

#define mm256_rotate_left(data, count)                                                             \
  _mm256_or_si256(                                                                                 \
      _mm256_slli_epi64(data, count),                                                              \
      _mm256_permute4x64_epi64(_mm256_srli_epi64(data, 64 - count), _MM_SHUFFLE(2, 1, 0, 3)))

#define mm256_rotate_right(data, count)                                                            \
  _mm256_or_si256(                                                                                 \
      _mm256_srli_epi64(data, count),                                                              \
      _mm256_permute4x64_epi64(_mm256_slli_epi64(data, 64 - count), _MM_SHUFFLE(0, 3, 2, 1)))
#endif

#if defined(WITH_SSE2) || defined(WITH_AVX2)
typedef __m128i word128;

#define mm128_zero _mm_setzero_si128()
#define mm128_load(s) _mm_load_si128((const word128*)s)
#define mm128_store(d, s) _mm_store_si128((word128*)d, s)
#define mm128_xor(l, r) _mm_xor_si128((l), (r))
#define mm128_and(l, r) _mm_and_si128((l), (r))
#define mm128_broadcast_u64(x) _mm_set1_epi64x((x))
/* bit shifts up to 63 bits */
#define mm128_sl_u64(x, s) _mm_slli_epi64((x), (s))
#define mm128_sr_u64(x, s) _mm_srli_epi64((x), (s))

// clang-format off
apply_region(mm128_xor_region, word128, mm128_xor, FN_ATTRIBUTES_S128)
apply_mask_region(mm128_xor_mask_region, word128, mm128_load, mm128_xor, mm128_and, FN_ATTRIBUTES_S128)
apply_mask(mm128_xor_mask, word128, mm128_xor, mm128_and, FN_ATTRIBUTES_S128_CONST)
apply_array(mm128_xor_256, word128, mm128_xor, 2, FN_ATTRIBUTES_S128)
apply_array(mm128_and_256, word128, mm128_and, 2, FN_ATTRIBUTES_S128)
// clang-format on

#define mm128_shift_left(data, count)                                                              \
  _mm_or_si128(_mm_slli_epi64(data, count), _mm_srli_epi64(_mm_bslli_si128(data, 8), 64 - count))

#define mm128_shift_right(data, count)                                                             \
  _mm_or_si128(_mm_srli_epi64(data, count), _mm_slli_epi64(_mm_bsrli_si128(data, 8), 64 - count))

#define mm128_rotate_left(data, count)                                                             \
  _mm_or_si128(_mm_slli_epi64(data, count),                                                        \
               _mm_shuffle_epi32(_mm_srli_epi64(data, 64 - count), _MM_SHUFFLE(1, 0, 3, 2)))

#define mm128_rotate_right(data, count)                                                            \
  _mm_or_si128(_mm_srli_epi64(data, count),                                                        \
               _mm_shuffle_epi32(_mm_slli_epi64(data, 64 - count), _MM_SHUFFLE(1, 0, 3, 2)))

#define mm128_shift_right_256(res, data, count)                                                    \
  do {                                                                                             \
    const __m128i total_carry = _mm_slli_epi64(_mm_bslli_si128(data[1], 8), 64 - count);           \
    __m128i carry             = _mm_slli_epi64(_mm_bsrli_si128(data[0], 8), 64 - count);           \
    res[0]                    = _mm_or_si128(_mm_srli_epi64(data[0], count), carry);               \
    carry                     = _mm_slli_epi64(_mm_bsrli_si128(data[1], 8), 64 - count);           \
    res[1]                    = _mm_or_si128(_mm_srli_epi64(data[1], count), carry);               \
    res[0]                    = _mm_or_si128(res[0], total_carry);                                 \
  } while (0)

#define mm128_shift_left_256(res, data, count)                                                     \
  do {                                                                                             \
    const __m128i total_carry = _mm_srli_epi64(_mm_bsrli_si128(data[0], 8), 64 - count);           \
    __m128i carry             = _mm_srli_epi64(_mm_bslli_si128(data[0], 8), 64 - count);           \
    res[0]                    = _mm_or_si128(_mm_slli_epi64(data[0], count), carry);               \
    carry                     = _mm_srli_epi64(_mm_bslli_si128(data[1], 8), 64 - count);           \
    res[1]                    = _mm_or_si128(_mm_slli_epi64(data[1], count), carry);               \
    res[1]                    = _mm_or_si128(res[1], total_carry);                                 \
  } while (0)

/* shift left by 64 to 127 bits */
#define mm128_shift_left_64_127(data, count) _mm_slli_epi64(_mm_bslli_si128(data, 8), count - 64)
/* shift right by 64 to 127 bits */
#define mm128_shift_right_64_127(data, count) _mm_srli_epi64(_mm_bsrli_si128(data, 8), count - 64)

#define mm128_rotate_left_256(res, data, count)                                                    \
  do {                                                                                             \
    const __m128i carry = mm128_shift_right_64_127(data[0], 128 - count);                          \
                                                                                                   \
    res[0] = _mm_or_si128(mm128_shift_left(data[0], count),                                        \
                          mm128_shift_right_64_127(data[1], 128 - count));                         \
    res[1] = _mm_or_si128(mm128_shift_left(data[1], count), carry);                                \
  } while (0)

#define mm128_rotate_right_256(res, data, count)                                                   \
  do {                                                                                             \
    const __m128i carry = mm128_shift_left_64_127(data[0], 128 - count);                           \
                                                                                                   \
    res[0] = _mm_or_si128(mm128_shift_right(data[0], count),                                       \
                          mm128_shift_left_64_127(data[1], 128 - count));                          \
    res[1] = _mm_or_si128(mm128_shift_right(data[1], count), carry);                               \
  } while (0)
#endif

#if defined(WITH_NEON)
typedef uint64x2_t word128;

#define mm128_zero vmovq_n_u64(0)
#define mm128_load(s) vld1q_u64(s)
#define mm128_store(d, s) vst1q_u64(d, s)
#define mm128_xor(l, r) veorq_u64((l), (r))
#define mm128_and(l, r) vandq_u64((l), (r))
#define mm128_broadcast_u64(x) vdupq_n_u64((x))
/* bit shifts up to 63 bits */
#define mm128_sl_u64(x, s) vshlq_n_u64((x), (s))
#define mm128_sr_u64(x, s) vshrq_n_u64((x), (s))

// clang-format off
apply_region(mm128_xor_region, word128, mm128_xor, FN_ATTRIBUTES_S128)
apply_mask_region(mm128_xor_mask_region, word128, mm128_xor, mm128_and, FN_ATTRIBUTES_S128)
apply_mask(mm128_xor_mask, word128, mm128_xor, mm128_and, FN_ATTRIBUTES_S128_CONST)
apply_array(mm128_xor_256, word128, mm128_xor, 2, FN_ATTRIBUTES_S128)
apply_array(mm128_and_256, word128, mm128_and, 2, FN_ATTRIBUTES_S128)
// clang-format on

/* shift left by 64 to 127 bits */
#define mm128_shift_left_64_127(data, count)                                                       \
  mm128_sl_u64(vextq_u64(mm128_zero, data, 1), count - 64)
/* shift right by 64 to 127 bits */
#define mm128_shift_right_64_127(data, count)                                                      \
  mm128_sr_u64(vextq_u64(data, mm128_zero, 1), count - 64)

#define mm128_shift_left(data, count)                                                              \
  vorrq_u64(mm128_sl_u64(data, count), mm128_sr_u64(vextq_u64(mm128_zero, data, 1), 64 - count))

#define mm128_shift_right(data, count)                                                             \
  vorrq_u64(mm128_sr_u64(data, count), mm128_sl_u64(vextq_u64(data, mm128_zero, 1), 64 - count))

#define mm128_rotate_left(data, count)                                                             \
  vorrq_u64(mm128_shift_left(data, count), mm128_shift_right_64_127(data, 128 - count))

#define mm128_rotate_right(data, count)                                                            \
  vorrq_u64(mm128_shift_right(data, count), mm128_shift_left_64_127(data, 128 - count))

#define mm128_shift_left_256(res, data, count)                                                     \
  do {                                                                                             \
    res[1] = vorrq_u64(mm128_shift_left(data[1], count),                                           \
                       mm128_shift_right_64_127(data[0], 128 - count));                            \
    res[0] = mm128_shift_left(data[0], count);                                                     \
  } while (0)

#define mm128_shift_right_256(res, data, count)                                                    \
  do {                                                                                             \
    res[0] = vorrq_u64(mm128_shift_right(data[0], count),                                          \
                       mm128_shift_left_64_127(data[1], 128 - count));                             \
    res[1] = mm128_shift_right(data[1], count);                                                    \
  } while (0)

#define mm128_rotate_left_256(res, data, count)                                                    \
  do {                                                                                             \
    const word128 carry = mm128_shift_right_64_127(data[1], 128 - count);                          \
                                                                                                   \
    res[1] = vorrq_u64(mm128_shift_left(data[1], count),                                           \
                       mm128_shift_right_64_127(data[0], 128 - count));                            \
    res[0] = vorrq_u64(mm128_shift_left(data[0], count), carry);                                   \
  } while (0)

#define mm128_rotate_right_256(res, data, count)                                                   \
  do {                                                                                             \
    const word128 carry = mm128_shift_left_64_127(data[0], 128 - count);                           \
                                                                                                   \
    res[0] = vorrq_u64(mm128_shift_right(data[0], count),                                          \
                       mm128_shift_left_64_127(data[1], 128 - count));                             \
    res[1] = vorrq_u64(mm128_shift_right(data[1], count), carry);                                  \
  } while (0)
#endif

#if defined(_MSC_VER)
#undef restrict
#endif

#undef apply_region
#undef apply_mask_region
#undef apply_array
#undef BUILTIN_CPU_SUPPORTED

#endif
