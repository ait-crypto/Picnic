/*
 *  This file is part of the optimized implementation of the Picnic signature scheme.
 *  See the accompanying documentation for complete details.
 *
 *  The code is provided under the MIT license, see LICENSE for
 *  more details.
 *  SPDX-License-Identifier: MIT
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "picnic_instances.h"

// instance handling

// L1, L3, and L5 instances with partial Sbox layer
#if defined(WITH_LOWMC_128_128_20)
#include "lowmc_128_128_20.h"
#else
#define lowmc_parameters_128_128_20                                                                \
  { 0, 0, 0, 0 }
#endif
#if defined(WITH_LOWMC_192_192_30)
#include "lowmc_192_192_30.h"
#else
#define lowmc_parameters_192_192_30                                                                \
  { 0, 0, 0, 0 }
#endif
#if defined(WITH_LOWMC_256_256_38)
#include "lowmc_256_256_38.h"
#else
#define lowmc_parameters_256_256_38                                                                \
  { 0, 0, 0, 0 }
#endif

// L1, L3, and L5 instances with full Sbox layer
#if defined(WITH_LOWMC_129_129_4)
#include "lowmc_129_129_4.h"
#else
#define lowmc_parameters_129_129_4                                                                 \
  { 0, 0, 0, 0 }
#endif
#if defined(WITH_LOWMC_192_192_4)
#include "lowmc_192_192_4.h"
#else
#define lowmc_parameters_192_192_4                                                                 \
  { 0, 0, 0, 0 }
#endif
#if defined(WITH_LOWMC_255_255_4)
#include "lowmc_255_255_4.h"
#else
#define lowmc_parameters_255_255_4                                                                 \
  { 0, 0, 0, 0 }
#endif

#if defined(WITH_ZKBPP)
#define PARAMETER_SET_ZKBPP(params, digest_size, seed_size, num_rounds, input_size, output_size,   \
                            view_size, unruh_without_input_bytes_size)                             \
  {                                                                                                \
    num_rounds, digest_size, seed_size, input_size, output_size, view_size, 0, 0,                  \
        unruh_without_input_bytes_size, params                                                     \
  }
#else
#define PARAMETER_SET_ZKBPP(params, digest_size, seed_size, num_rounds, input_size, output_size,   \
                            view_size, unruh_without_input_bytes_size)                             \
  {                                                                                                \
    num_rounds, digest_size, seed_size, input_size, output_size, view_size, 0, 0,                  \
        unruh_without_input_bytes_size, {                                                          \
      0, 0, 0, 0                                                                                   \
    }                                                                                              \
  }
#endif

#if defined(WITH_KKW)
#define PARAMETER_SET_KKW(params, digest_size, seed_size, num_rounds, num_opened_rounds,           \
                          num_MPC_parties, input_size, output_size, view_size)                     \
  {                                                                                                \
    num_rounds, digest_size, seed_size, input_size, output_size, view_size, num_opened_rounds,     \
        num_MPC_parties, 0, params,                                                                \
  }
#else
#define PARAMETER_SET_KKW(params, digest_size, seed_size, num_rounds, num_opened_rounds,           \
                          num_MPC_parties, input_size, output_size, view_size)                     \
  {                                                                                                \
    num_rounds, digest_size, seed_size, input_size, output_size, view_size, num_opened_rounds,     \
        num_MPC_parties, 0, {                                                                      \
      0, 0, 0, 0                                                                                   \
    }                                                                                              \
  }

#endif

static const picnic_instance_t instances[PARAMETER_SET_MAX_INDEX - 1] = {
    /* ZKB++ with partial LowMC instances */
    PARAMETER_SET_ZKBPP(lowmc_parameters_128_128_20, 32, 16, 219, 16, 16, 75, 0),
    PARAMETER_SET_ZKBPP(lowmc_parameters_128_128_20, 32, 16, 219, 16, 16, 75, 91),
    PARAMETER_SET_ZKBPP(lowmc_parameters_192_192_30, 48, 24, 329, 24, 24, 113, 0),
    PARAMETER_SET_ZKBPP(lowmc_parameters_192_192_30, 48, 24, 329, 24, 24, 113, 137),
    PARAMETER_SET_ZKBPP(lowmc_parameters_256_256_38, 64, 32, 438, 32, 32, 143, 0),
    PARAMETER_SET_ZKBPP(lowmc_parameters_256_256_38, 64, 32, 438, 32, 32, 143, 175),
    /* KKW with full LowMC instances */
    PARAMETER_SET_KKW(lowmc_parameters_129_129_4, 32, 16, 250, 36, 16, 17, 17, 65),
    PARAMETER_SET_KKW(lowmc_parameters_192_192_4, 48, 24, 419, 52, 16, 24, 24, 96),
    PARAMETER_SET_KKW(lowmc_parameters_255_255_4, 64, 32, 601, 68, 16, 32, 32, 128),
    /* ZKB++ with full LowMC instances */
    PARAMETER_SET_ZKBPP(lowmc_parameters_129_129_4, 32, 16, 219, 17, 17, 65, 0),
    PARAMETER_SET_ZKBPP(lowmc_parameters_192_192_4, 48, 24, 329, 24, 24, 96, 0),
    PARAMETER_SET_ZKBPP(lowmc_parameters_255_255_4, 64, 32, 438, 32, 32, 128, 0),
};

const picnic_instance_t* picnic_instance_get(picnic_params_t param) {
  if (param <= PARAMETER_SET_INVALID || param >= PARAMETER_SET_MAX_INDEX) {
    return NULL;
  }

  const picnic_instance_t* pp = &instances[param - 1];
  if (!pp->lowmc.n) {
    return NULL;
  }

  return pp;
}
