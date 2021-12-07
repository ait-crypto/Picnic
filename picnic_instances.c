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
#define lowmc_parameters_128_128_20 { 0, 0, 0, 0 }
#endif
#if defined(WITH_LOWMC_192_192_30)
#include "lowmc_192_192_30.h"
#else
#define lowmc_parameters_192_192_30 { 0, 0, 0, 0 }
#endif
#if defined(WITH_LOWMC_256_256_38)
#include "lowmc_256_256_38.h"
#else
#define lowmc_parameters_256_256_38 { 0, 0, 0, 0 }
#endif

// L1, L3, and L5 instances with full Sbox layer
#if defined(WITH_LOWMC_129_129_4)
#include "lowmc_129_129_4.h"
#else
#define lowmc_parameters_129_129_4 { 0, 0, 0, 0 }
#endif
#if defined(WITH_LOWMC_192_192_4)
#include "lowmc_192_192_4.h"
#else
#define lowmc_parameters_192_192_4 { 0, 0, 0, 0 }
#endif
#if defined(WITH_LOWMC_255_255_4)
#include "lowmc_255_255_4.h"
#else
#define lowmc_parameters_255_255_4 { 0, 0, 0, 0 }
#endif

#if defined(WITH_ZKBPP)
#define ENABLE_ZKBPP(x) x
#else
#define ENABLE_ZKBPP(x) { 0, 0, 0, 0 }
#endif

#if defined(WITH_KKW)
#define ENABLE_KKW(x) x
#else
#define ENABLE_KKW(x) { 0, 0, 0, 0 }
#endif

#if defined(WITH_ZKBPP) && defined(WITH_KKW)
#define NULL_FNS                                                                                   \
  { NULL, NULL, NULL, NULL, NULL, NULL, NULL }
#elif defined(WITH_ZKBPP)
#define NULL_FNS                                                                                   \
  { NULL, NULL, NULL, NULL, NULL }
#elif defined(WITH_KKW)
#define NULL_FNS                                                                                   \
  { NULL, NULL, NULL }
#else
#error "At least one of WITH_ZKBPP and WITH_KKW have to be defined!"
#endif

static picnic_instance_t instances[PARAMETER_SET_MAX_INDEX] = {
    {{0, 0, 0, 0}, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL_FNS},
    /* ZKB++ with partial LowMC instances */
    {ENABLE_ZKBPP(lowmc_parameters_128_128_20), 32, 16, 219, 219, 3, 16, 16, 75, 30, 55, 0, 0,
     PICNIC_SIGNATURE_SIZE_Picnic_L1_FS, NULL_FNS},
    {ENABLE_ZKBPP(lowmc_parameters_128_128_20), 32, 16, 219, 219, 3, 16, 16, 75, 30, 55, 91, 107,
     PICNIC_SIGNATURE_SIZE_Picnic_L1_UR, NULL_FNS},
    {ENABLE_ZKBPP(lowmc_parameters_192_192_30), 48, 24, 329, 329, 3, 24, 24, 113, 30, 83, 0, 0,
     PICNIC_SIGNATURE_SIZE_Picnic_L3_FS, NULL_FNS},
    {ENABLE_ZKBPP(lowmc_parameters_192_192_30), 48, 24, 329, 329, 3, 24, 24, 113, 30, 83, 137, 161,
     PICNIC_SIGNATURE_SIZE_Picnic_L3_UR, NULL_FNS},
    {ENABLE_ZKBPP(lowmc_parameters_256_256_38), 64, 32, 438, 438, 3, 32, 32, 143, 30, 110, 0, 0,
     PICNIC_SIGNATURE_SIZE_Picnic_L5_FS, NULL_FNS},
    {ENABLE_ZKBPP(lowmc_parameters_256_256_38), 64, 32, 438, 438, 3, 32, 32, 143, 30, 110, 175, 207,
     PICNIC_SIGNATURE_SIZE_Picnic_L5_UR, NULL_FNS},
    /* KKW with full LowMC instances */
    {ENABLE_KKW(lowmc_parameters_129_129_4), 32, 16, 250, 36, 16, 17, 17, 65, 129, 55, 0, 0,
     PICNIC_SIGNATURE_SIZE_Picnic3_L1, NULL_FNS},
    {ENABLE_KKW(lowmc_parameters_192_192_4), 48, 24, 419, 52, 16, 24, 24, 96, 192, 83, 0, 0,
     PICNIC_SIGNATURE_SIZE_Picnic3_L3, NULL_FNS},
    {ENABLE_KKW(lowmc_parameters_255_255_4), 64, 32, 601, 68, 16, 32, 32, 128, 255, 110, 0, 0,
     PICNIC_SIGNATURE_SIZE_Picnic3_L5, NULL_FNS},
    /* ZKB++ with full LowMC instances */
    {ENABLE_ZKBPP(lowmc_parameters_129_129_4), 32, 16, 219, 219, 3, 17, 17, 65, 129, 55, 0, 0,
     PICNIC_SIGNATURE_SIZE_Picnic_L1_full, NULL_FNS},
    {ENABLE_ZKBPP(lowmc_parameters_192_192_4), 48, 24, 329, 329, 3, 24, 24, 96, 192, 83, 0, 0,
     PICNIC_SIGNATURE_SIZE_Picnic_L3_full, NULL_FNS},
    {ENABLE_ZKBPP(lowmc_parameters_255_255_4), 64, 32, 438, 438, 3, 32, 32, 128, 255, 110, 0, 0,
     PICNIC_SIGNATURE_SIZE_Picnic_L5_full, NULL_FNS},
};

static bool create_instance(picnic_params_t params, picnic_instance_t* pp) {
  if (!pp->lowmc.m || !pp->lowmc.n || !pp->lowmc.r || !pp->lowmc.k) {
    return false;
  }

#if !defined(WITH_UNRUH)
  if (params == Picnic_L1_UR || params == Picnic_L3_UR || params == Picnic_L5_UR) {
    return false;
  }
#endif

  pp->impls.lowmc = lowmc_get_implementation(&pp->lowmc);
#if defined(WITH_ZKBPP)
  if ((params >= Picnic_L1_FS && params <= Picnic_L5_UR) ||
      (params >= Picnic_L1_full && params <= Picnic_L5_full)) {
    pp->impls.lowmc_store        = lowmc_store_get_implementation(&pp->lowmc);
    pp->impls.zkbpp_lowmc        = get_zkbpp_lowmc_implementation(&pp->lowmc);
    pp->impls.zkbpp_lowmc_verify = get_zkbpp_lowmc_verify_implementation(&pp->lowmc);
    pp->impls.mzd_share          = get_zkbpp_share_implentation(&pp->lowmc);
  }
#endif
#if defined(WITH_KKW)
  if (params >= Picnic3_L1 && params <= Picnic3_L5) {
    pp->impls.lowmc_aux             = lowmc_compute_aux_get_implementation(&pp->lowmc);
    pp->impls.lowmc_simulate_online = lowmc_simulate_online_get_implementation(&pp->lowmc);
  }
#endif

  return true;
}

const picnic_instance_t* picnic_instance_get(picnic_params_t param) {
  if (param <= PARAMETER_SET_INVALID || param >= PARAMETER_SET_MAX_INDEX) {
    return NULL;
  }

  picnic_instance_t* pp = &instances[param];
  if (!pp->impls.lowmc) {
    if (!create_instance(param, pp)) {
      return NULL;
    }
  }

  return pp;
}
