/*! @file picnic3_impl.c
 *  @brief This is the main file of the signature scheme for the Picnic3
 *  parameter sets.
 *
 *  This file is part of the reference implementation of the Picnic signature scheme.
 *  See the accompanying documentation for complete details.
 *
 *  The code is provided under the MIT license, see LICENSE for
 *  more details.
 *  SPDX-License-Identifier: MIT
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if !defined(_MSC_VER)
#include <stdalign.h>
#endif

#include "bitstream.h"
#include "io.h"
#include "picnic3_simulate.h"
#include "picnic3_types.h"

#define picnic3_mpc_sbox_bitsliced(LOWMC_N, XOR, AND, SHIFT_LEFT, SHIFT_RIGHT, bitmask_a,          \
                                   bitmask_b, bitmask_c)                                           \
  do {                                                                                             \
    mzd_local_t a[1], b[1], c[1];                                                                  \
    /* a */                                                                                        \
    AND(a, bitmask_a, statein);                                                                    \
    /* b */                                                                                        \
    AND(b, bitmask_b, statein);                                                                    \
    /* c */                                                                                        \
    AND(c, bitmask_c, statein);                                                                    \
                                                                                                   \
    SHIFT_LEFT(a, a, 2);                                                                           \
    SHIFT_LEFT(b, b, 1);                                                                           \
                                                                                                   \
    mzd_local_t t0[1], t1[1], t2[1];                                                               \
                                                                                                   \
    mzd_local_t s_ab[1] = {{{0, 0, 0, 0}}}, s_bc[1] = {{{0, 0, 0, 0}}},                            \
                s_ca[1] = {{{0, 0, 0, 0}}};                                                        \
    for (int i = 0; i < 16; i++) {                                                                 \
      mzd_local_t tmp[1];                                                                          \
      bitstream_t party_msgs = {{msgs->msgs[i]}, msgs->pos};                                       \
      if (i == msgs->unopened) {                                                                   \
        /* we are in verify, just grab the broadcast s from the msgs array */                      \
        mzd_from_bitstream(&party_msgs, tmp, (LOWMC_N + 63) / (sizeof(uint64_t) * 8), LOWMC_N);    \
        /* a */                                                                                    \
        AND(t0, bitmask_a, tmp);                                                                   \
        /* b */                                                                                    \
        AND(t1, bitmask_b, tmp);                                                                   \
        /* c */                                                                                    \
        AND(t2, bitmask_c, tmp);                                                                   \
        SHIFT_LEFT(t0, t0, 2);                                                                     \
        SHIFT_LEFT(t1, t1, 1);                                                                     \
        XOR(s_ab, t2, s_ab);                                                                       \
        XOR(s_bc, t1, s_bc);                                                                       \
        XOR(s_ca, t0, s_ca);                                                                       \
                                                                                                   \
        continue;                                                                                  \
      }                                                                                            \
      bitstream_t party_tape = {{tapes->tape[i]}, tapes->pos};                                     \
      /* make a mzd_local from tape[i] for input_masks */                                          \
      mzd_local_t mask_a[1], mask_b[1], mask_c[1];                                                 \
      mzd_from_bitstream(&party_tape, tmp, (LOWMC_N + 63) / (sizeof(uint64_t) * 8), LOWMC_N);      \
      /* a */                                                                                      \
      AND(mask_a, bitmask_a, tmp);                                                                 \
      /* b */                                                                                      \
      AND(mask_b, bitmask_b, tmp);                                                                 \
      /* c */                                                                                      \
      AND(mask_c, bitmask_c, tmp);                                                                 \
      SHIFT_LEFT(mask_a, mask_a, 2);                                                               \
      SHIFT_LEFT(mask_b, mask_b, 1);                                                               \
                                                                                                   \
      /* make a mzd_local from tape[i] for and_helper */                                           \
      mzd_local_t and_helper_ab[1], and_helper_bc[1], and_helper_ca[1];                            \
      mzd_from_bitstream(&party_tape, tmp, (LOWMC_N + 63) / (sizeof(uint64_t) * 8), LOWMC_N);      \
      /* a */                                                                                      \
      AND(and_helper_ab, bitmask_c, tmp);                                                          \
      /* b */                                                                                      \
      AND(and_helper_bc, bitmask_b, tmp);                                                          \
      /* c */                                                                                      \
      AND(and_helper_ca, bitmask_a, tmp);                                                          \
      SHIFT_LEFT(and_helper_ca, and_helper_ca, 2);                                                 \
      SHIFT_LEFT(and_helper_bc, and_helper_bc, 1);                                                 \
                                                                                                   \
      /* s_ab */                                                                                   \
      AND(t0, a, mask_b);                                                                          \
      AND(t1, b, mask_a);                                                                          \
      XOR(t0, t0, t1);                                                                             \
      XOR(tmp, t0, and_helper_ab);                                                                 \
      XOR(s_ab, tmp, s_ab);                                                                        \
      /* s_bc */                                                                                   \
      AND(t0, b, mask_c);                                                                          \
      AND(t1, c, mask_b);                                                                          \
      XOR(t0, t0, t1);                                                                             \
      XOR(t0, t0, and_helper_bc);                                                                  \
      XOR(s_bc, t0, s_bc);                                                                         \
                                                                                                   \
      SHIFT_RIGHT(t0, t0, 1);                                                                      \
      XOR(tmp, tmp, t0);                                                                           \
      /* s_ca */                                                                                   \
      AND(t0, c, mask_a);                                                                          \
      AND(t1, a, mask_c);                                                                          \
      XOR(t0, t0, t1);                                                                             \
      XOR(t0, t0, and_helper_ca);                                                                  \
      XOR(s_ca, t0, s_ca);                                                                         \
                                                                                                   \
      SHIFT_RIGHT(t0, t0, 2);                                                                      \
      XOR(tmp, tmp, t0);                                                                           \
      mzd_to_bitstream(&party_msgs, tmp, (LOWMC_N + 63) / (sizeof(uint64_t) * 8), LOWMC_N);        \
    }                                                                                              \
    tapes->pos += LOWMC_N;                                                                         \
    tapes->pos += LOWMC_N;                                                                         \
    msgs->pos += LOWMC_N;                                                                          \
    /* b & c */                                                                                    \
    AND(t0, b, c);                                                                                 \
    /* c & a */                                                                                    \
    AND(t1, c, a);                                                                                 \
    /* a & b */                                                                                    \
    AND(t2, a, b);                                                                                 \
    /* (b & c) ^ s_bc */                                                                           \
    XOR(t0, t0, s_bc);                                                                             \
    /* (c & a) ^ s_ca */                                                                           \
    XOR(t1, t1, s_ca);                                                                             \
    /* (a & b) ^ s_ab */                                                                           \
    XOR(t2, t2, s_ab);                                                                             \
                                                                                                   \
    /* (b & c) ^ a */                                                                              \
    XOR(t0, t0, a);                                                                                \
                                                                                                   \
    /* (c & a) ^ a ^ b */                                                                          \
    XOR(t1, t1, a);                                                                                \
    XOR(t1, t1, b);                                                                                \
                                                                                                   \
    /* (a & b) ^ a ^ b ^c */                                                                       \
    XOR(t2, t2, a);                                                                                \
    XOR(t2, t2, b);                                                                                \
    XOR(t2, t2, c);                                                                                \
                                                                                                   \
    SHIFT_RIGHT(t0, t0, 2);                                                                        \
    SHIFT_RIGHT(t1, t1, 1);                                                                        \
                                                                                                   \
    XOR(t2, t2, t1);                                                                               \
    XOR(statein, t2, t0);                                                                          \
  } while (0)

#if defined(WITH_LOWMC_129_129_4)
#include "lowmc_129_129_4.h"
#endif
#if defined(WITH_LOWMC_192_192_4)
#include "lowmc_192_192_4.h"
#endif
#if defined(WITH_LOWMC_255_255_4)
#include "lowmc_255_255_4.h"
#endif

#if !defined(NO_UINT64_FALLBACK)
static void picnic3_mpc_sbox_uint64_lowmc_129_129_4(mzd_local_t* statein, randomTape_t* tapes,
                                                    msgs_t* msgs) {

  picnic3_mpc_sbox_bitsliced(LOWMC_129_129_4_N, mzd_xor_uint64_192, mzd_and_uint64_192,
                             mzd_shift_left_uint64_192, mzd_shift_right_uint64_192,
                             mask_129_129_43_a, mask_129_129_43_b, mask_129_129_43_c);
}

static void picnic3_mpc_sbox_uint64_lowmc_192_192_4(mzd_local_t* statein, randomTape_t* tapes,
                                                    msgs_t* msgs) {

  picnic3_mpc_sbox_bitsliced(LOWMC_192_192_4_N, mzd_xor_uint64_192, mzd_and_uint64_192,
                             mzd_shift_left_uint64_192, mzd_shift_right_uint64_192,
                             mask_192_192_64_a, mask_192_192_64_b, mask_192_192_64_c);
}

static void picnic3_mpc_sbox_uint64_lowmc_255_255_4(mzd_local_t* statein, randomTape_t* tapes,
                                                    msgs_t* msgs) {

  picnic3_mpc_sbox_bitsliced(LOWMC_255_255_4_N, mzd_xor_uint64_256, mzd_and_uint64_256,
                             mzd_shift_left_uint64_256, mzd_shift_right_uint64_256,
                             mask_255_255_85_a, mask_255_255_85_b, mask_255_255_85_c);
}
#define IMPL uint64
/* PICNIC3_L1_FS */
#include "lowmc_129_129_4_fns_uint64.h"
#undef SIM_ONLINE
#define SIM_ONLINE lowmc_simulate_online_uint64_129_43
#include "picnic3_simulate.c.i"

/* PICNIC3_L3_FS */
#include "lowmc_192_192_4_fns_uint64.h"
#undef SIM_ONLINE
#define SIM_ONLINE lowmc_simulate_online_uint64_192_64
#include "picnic3_simulate.c.i"

/* PICNIC3_L5_FS */
#include "lowmc_255_255_4_fns_uint64.h"
#undef SIM_ONLINE
#define SIM_ONLINE lowmc_simulate_online_uint64_255_85
#include "picnic3_simulate.c.i"
#undef IMPL
#endif

#if defined(WITH_OPT)
#if defined(WITH_SSE2) || defined(WITH_NEON)
static void picnic3_mpc_sbox_s128_lowmc_129_129_4(mzd_local_t* statein, randomTape_t* tapes,
                                                  msgs_t* msgs) {
  picnic3_mpc_sbox_bitsliced(LOWMC_129_129_4_N, mzd_xor_s128_256, mzd_and_s128_256,
                             mzd_shift_left_uint64_192, mzd_shift_right_uint64_192,
                             mask_129_129_43_a, mask_129_129_43_b, mask_129_129_43_c);
}

static void picnic3_mpc_sbox_s128_lowmc_192_192_4(mzd_local_t* statein, randomTape_t* tapes,
                                                  msgs_t* msgs) {
  picnic3_mpc_sbox_bitsliced(LOWMC_192_192_4_N, mzd_xor_s128_256, mzd_and_s128_256,
                             mzd_shift_left_uint64_192, mzd_shift_right_uint64_192,
                             mask_192_192_64_a, mask_192_192_64_b, mask_192_192_64_c);
}

static void picnic3_mpc_sbox_s128_lowmc_255_255_4(mzd_local_t* statein, randomTape_t* tapes,
                                                  msgs_t* msgs) {
  picnic3_mpc_sbox_bitsliced(LOWMC_255_255_4_N, mzd_xor_s128_256, mzd_and_s128_256,
                             mzd_shift_left_uint64_256, mzd_shift_right_uint64_256,
                             mask_255_255_85_a, mask_255_255_85_b, mask_255_255_85_c);
}
#define IMPL s128
#undef FN_ATTR
#if defined(WITH_SSE2)
#define FN_ATTR ATTR_TARGET_SSE2
#endif
/* PICNIC3_L1_FS */
#include "lowmc_129_129_4_fns_s128.h"
#undef SIM_ONLINE
#define SIM_ONLINE lowmc_simulate_online_s128_129_43
#include "picnic3_simulate.c.i"

/* PICNIC3_L3_FS */
#include "lowmc_192_192_4_fns_s128.h"
#undef SIM_ONLINE
#define SIM_ONLINE lowmc_simulate_online_s128_192_64
#include "picnic3_simulate.c.i"

/* PICNIC3_L5_FS */
#include "lowmc_255_255_4_fns_s128.h"
#undef SIM_ONLINE
#define SIM_ONLINE lowmc_simulate_online_s128_255_85
#include "picnic3_simulate.c.i"

#undef IMPL
#endif // SSE/NEON

#if defined(WITH_AVX2)
static void picnic3_mpc_sbox_s256_lowmc_129_129_4(mzd_local_t* statein, randomTape_t* tapes,
                                                  msgs_t* msgs) {
  picnic3_mpc_sbox_bitsliced(LOWMC_129_129_4_N, mzd_xor_s256_256, mzd_and_s256_256,
                             mzd_shift_left_uint64_192, mzd_shift_right_uint64_192,
                             mask_129_129_43_a, mask_129_129_43_b, mask_129_129_43_c);
}

static void picnic3_mpc_sbox_s256_lowmc_192_192_4(mzd_local_t* statein, randomTape_t* tapes,
                                                  msgs_t* msgs) {
  picnic3_mpc_sbox_bitsliced(LOWMC_192_192_4_N, mzd_xor_s256_256, mzd_and_s256_256,
                             mzd_shift_left_uint64_192, mzd_shift_right_uint64_192,
                             mask_192_192_64_a, mask_192_192_64_b, mask_192_192_64_c);
}

static void picnic3_mpc_sbox_s256_lowmc_255_255_4(mzd_local_t* statein, randomTape_t* tapes,
                                                  msgs_t* msgs) {
  picnic3_mpc_sbox_bitsliced(LOWMC_255_255_4_N, mzd_xor_s256_256, mzd_and_s256_256,
                             mzd_shift_left_uint64_256, mzd_shift_right_uint64_256,
                             mask_255_255_85_a, mask_255_255_85_b, mask_255_255_85_c);
}
#define IMPL s256
#undef FN_ATTR
#define FN_ATTR ATTR_TARGET_AVX2
/* PICNIC3_L1_FS */
#include "lowmc_129_129_4_fns_s256.h"
#undef SIM_ONLINE
#define SIM_ONLINE lowmc_simulate_online_s256_129_43
#include "picnic3_simulate.c.i"

/* PICNIC3_L3_FS */
#include "lowmc_192_192_4_fns_s256.h"
#undef SIM_ONLINE
#define SIM_ONLINE lowmc_simulate_online_s256_192_64
#include "picnic3_simulate.c.i"

/* PICNIC3_L5_FS */
#include "lowmc_255_255_4_fns_s256.h"
#undef SIM_ONLINE
#define SIM_ONLINE lowmc_simulate_online_s256_255_85
#include "picnic3_simulate.c.i"

#undef IMPL
#endif // AVX2
#endif // WITH_OPT

lowmc_simulate_online_f lowmc_simulate_online_get_implementation(const lowmc_parameters_t* lowmc) {
  assert((lowmc->m == 43 && lowmc->n == 129) || (lowmc->m == 64 && lowmc->n == 192) ||
         (lowmc->m == 85 && lowmc->n == 255));

#if defined(WITH_OPT)
#if defined(WITH_AVX2)
  if (CPU_SUPPORTS_AVX2) {
#if defined(WITH_LOWMC_129_129_4)
    if (lowmc->n == 129 && lowmc->m == 43)
      return lowmc_simulate_online_s256_129_43;
#endif
#if defined(WITH_LOWMC_192_192_4)
    if (lowmc->n == 192 && lowmc->m == 64)
      return lowmc_simulate_online_s256_192_64;
#endif
#if defined(WITH_LOWMC_255_255_4)
    if (lowmc->n == 255 && lowmc->m == 85)
      return lowmc_simulate_online_s256_255_85;
#endif
  }
#endif

#if defined(WITH_SSE2) || defined(WITH_NEON)
  if (CPU_SUPPORTS_SSE2 || CPU_SUPPORTS_NEON) {
#if defined(WITH_LOWMC_129_129_4)
    if (lowmc->n == 129 && lowmc->m == 43)
      return lowmc_simulate_online_s128_129_43;
#endif
#if defined(WITH_LOWMC_192_192_4)
    if (lowmc->n == 192 && lowmc->m == 64)
      return lowmc_simulate_online_s128_192_64;
#endif
#if defined(WITH_LOWMC_255_255_4)
    if (lowmc->n == 255 && lowmc->m == 85)
      return lowmc_simulate_online_s128_255_85;
#endif
  }
#endif
#endif

#if !defined(NO_UINT64_FALLBACK)
#if defined(WITH_LOWMC_129_129_4)
  if (lowmc->n == 129 && lowmc->m == 43)
    return lowmc_simulate_online_uint64_129_43;
#endif
#if defined(WITH_LOWMC_192_192_4)
  if (lowmc->n == 192 && lowmc->m == 64)
    return lowmc_simulate_online_uint64_192_64;
#endif
#if defined(WITH_LOWMC_255_255_4)
  if (lowmc->n == 255 && lowmc->m == 85)
    return lowmc_simulate_online_uint64_255_85;
#endif
#endif

  return NULL;
}
