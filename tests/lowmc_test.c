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

#include "../io.h"
#include "../lowmc.h"
#include "../macros.h"
#include "../picnic_instances.h"

#include <inttypes.h>
#include <stdint.h>

#include "utils.h"

static int lowmc_enc(const lowmc_parameters_t* lowmc, const uint8_t* key, const uint8_t* plaintext,
                     const uint8_t* expected) {
  if (!lowmc) {
    return -1;
  }

  const lowmc_implementation_f enc = lowmc_get_implementation(lowmc);
  if (!enc) {
    return 1;
  }

  mzd_local_t* sk  = mzd_local_init(1, lowmc->k);
  mzd_local_t* pt  = mzd_local_init(1, lowmc->n);
  mzd_local_t* ct  = mzd_local_init(1, lowmc->n);
  mzd_local_t* ctr = mzd_local_init(1, lowmc->n);

  mzd_from_char_array(sk, key, (lowmc->k + 7) / 8);
  mzd_from_char_array(pt, plaintext, (lowmc->n + 7) / 8);
  mzd_from_char_array(ct, expected, (lowmc->n + 7) / 8);

  enc(sk, pt, ctr);

  int ret = 0;
  if (!mzd_local_equal(ctr, ct, 1, lowmc->n)) {
    ret = 2;
  }

  mzd_local_free(ctr);
  mzd_local_free(ct);
  mzd_local_free(pt);
  mzd_local_free(sk);

  return ret;
}

#if defined(WITH_LOWMC_128_128_20)
#include "../lowmc_128_128_20.h"
static const lowmc_parameters_t parameters_128_128_20 = lowmc_parameters_128_128_20;

static int LowMC_test_vector_128_128_20_1(void) {
  const uint8_t key[16] = {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t plaintext[16] = {0xAB, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t ciphertext_expected[16] = {0x0E, 0x30, 0x72, 0x0B, 0x9F, 0x64, 0xD5, 0xC2,
                                           0xA7, 0x77, 0x1C, 0x8C, 0x23, 0x8D, 0x8F, 0x70};

  return lowmc_enc(&parameters_128_128_20, key, plaintext, ciphertext_expected);
}

static int LowMC_test_vector_128_128_20_2(void) {
  const uint8_t key[16] = {0xB5, 0xDF, 0x53, 0x7B, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t plaintext[16] = {0xF7, 0x7D, 0xB5, 0x7B, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t ciphertext_expected[16] = {0x0E, 0x59, 0x61, 0xE9, 0x99, 0x21, 0x53, 0xB1,
                                           0x32, 0x45, 0xAF, 0x24, 0x3D, 0xD7, 0xDD, 0xC0};

  return lowmc_enc(&parameters_128_128_20, key, plaintext, ciphertext_expected);
}

static int LowMC_test_vector_128_128_20_3(void) {
  const uint8_t key[16] = {0x08, 0x4c, 0x2a, 0x6e, 0x19, 0x5d, 0x3b, 0x7f,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t plaintext[16] = {0xf7, 0xb3, 0xd5, 0x91, 0xe6, 0xa2, 0xc4, 0x80,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t ciphertext_expected[16] = {0x91, 0x5c, 0x63, 0x21, 0xd7, 0x86, 0x46, 0xb6,
                                           0xc7, 0x65, 0x43, 0xff, 0xb8, 0x52, 0x3b, 0x4d};

  return lowmc_enc(&parameters_128_128_20, key, plaintext, ciphertext_expected);
}
#endif

#if defined(WITH_LOWMC_192_192_30)
#include "../lowmc_192_192_30.h"
static const lowmc_parameters_t parameters_192_192_30 = lowmc_parameters_192_192_30;

static int LowMC_test_vector_192_192_30_1(void) {
  const uint8_t key[24] = {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t plaintext[24] = {0xAB, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t ciphertext_expected[24] = {0xA8, 0x5B, 0x82, 0x44, 0x34, 0x4A, 0x2E, 0x1B,
                                           0x10, 0xA1, 0x7B, 0xAB, 0x04, 0x30, 0x73, 0xF6,
                                           0xBB, 0x64, 0x9A, 0xE6, 0xAF, 0x65, 0x9F, 0x6F};

  return lowmc_enc(&parameters_192_192_30, key, plaintext, ciphertext_expected);
}

static int LowMC_test_vector_192_192_30_2(void) {
  const uint8_t key[24] = {0xB5, 0xDF, 0x53, 0x7B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t plaintext[24] = {0xF7, 0x7D, 0xB5, 0x7B, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t ciphertext_expected[24] = {0x21, 0x0B, 0xBC, 0x4A, 0x43, 0x4B, 0x32, 0xDB,
                                           0x1E, 0x85, 0xAE, 0x7A, 0x27, 0xFE, 0xE9, 0xE4,
                                           0x15, 0x82, 0xFA, 0xC2, 0x1D, 0x03, 0x5A, 0xA1};

  return lowmc_enc(&parameters_192_192_30, key, plaintext, ciphertext_expected);
}

static int LowMC_test_vector_192_192_30_3(void) {
  const uint8_t key[24] = {0xF7, 0x7D, 0xB5, 0x7B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t plaintext[24] = {0xB5, 0xDF, 0x53, 0x7B, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t ciphertext_expected[24] = {0xE4, 0x82, 0xBC, 0xF9, 0xAD, 0x2C, 0x04, 0x48,
                                           0x31, 0x48, 0xD4, 0x6F, 0xBE, 0x1F, 0x8B, 0x51,
                                           0x46, 0x0D, 0xCC, 0x3E, 0x8E, 0xFB, 0x31, 0x01};

  return lowmc_enc(&parameters_192_192_30, key, plaintext, ciphertext_expected);
}
#endif

#if defined(WITH_LOWMC_256_256_38)
#include "../lowmc_256_256_38.h"
static const lowmc_parameters_t parameters_256_256_38 = lowmc_parameters_256_256_38;

static int LowMC_test_vector_256_256_38_1(void) {
  const uint8_t key[32] = {0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t plaintext[32] = {0xAB, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t ciphertext_expected[32] = {0xB8, 0xF2, 0x0A, 0x88, 0x8A, 0x0A, 0x9E, 0xC4,
                                           0xE4, 0x95, 0xF1, 0xFB, 0x43, 0x9A, 0xBD, 0xDE,
                                           0x18, 0xC1, 0xD3, 0xD2, 0x9C, 0xF2, 0x0D, 0xF4,
                                           0xB1, 0x0A, 0x56, 0x7A, 0xA0, 0x2C, 0x72, 0x67};

  return lowmc_enc(&parameters_256_256_38, key, plaintext, ciphertext_expected);
}

static int LowMC_test_vector_256_256_38_2(void) {
  const uint8_t key[32] = {0xF7, 0x7D, 0xB5, 0x7B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t plaintext[32] = {0xB5, 0xDF, 0x53, 0x7B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t ciphertext_expected[32] = {0xEE, 0xEC, 0xCE, 0x6A, 0x58, 0x4A, 0x93, 0x30,
                                           0x6D, 0xAE, 0xA0, 0x75, 0x19, 0xB4, 0x7A, 0xD6,
                                           0x40, 0x2C, 0x11, 0xDD, 0x94, 0x2A, 0xA3, 0x16,
                                           0x65, 0x41, 0x44, 0x49, 0x77, 0xA2, 0x14, 0xC5};

  return lowmc_enc(&parameters_256_256_38, key, plaintext, ciphertext_expected);
}

static int LowMC_test_vector_256_256_38_3(void) {
  const uint8_t key[32] = {0xB5, 0xDF, 0x53, 0x7B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t plaintext[32] = {0xF7, 0x7D, 0xB5, 0x7B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const uint8_t ciphertext_expected[32] = {0x03, 0x37, 0x33, 0x26, 0xC0, 0xF5, 0x0E, 0x3B,
                                           0x6B, 0x2E, 0x1C, 0xE8, 0xF9, 0x43, 0x0F, 0xF5,
                                           0xEB, 0x0E, 0xC3, 0x45, 0xC7, 0x27, 0xA4, 0x74,
                                           0x8F, 0xCF, 0x73, 0x17, 0x9D, 0x48, 0xE7, 0x9B};

  return lowmc_enc(&parameters_256_256_38, key, plaintext, ciphertext_expected);
}
#endif

#if defined(WITH_LOWMC_129_129_4)
#include "../lowmc_129_129_4.h"
static const lowmc_parameters_t parameters_129_129_4 = lowmc_parameters_129_129_4;

static int LowMC_test_vector_129_129_4_1(void) {

  const uint8_t key[17] = {
      0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  };
  const uint8_t plaintext[17] = {
      0xab, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  };
  const uint8_t ciphertext_expected[17] = {
      0x2f, 0xd7, 0xd5, 0x42, 0x5e, 0xe3, 0x5e, 0x66, 0x7c,
      0x97, 0x2f, 0x12, 0xfb, 0x15, 0x3e, 0x9d, 0x80,
  };

  return lowmc_enc(&parameters_129_129_4, key, plaintext, ciphertext_expected);
}

static int LowMC_test_vector_129_129_4_2(void) {
  const uint8_t key[17] = {
      0xab, 0x22, 0x42, 0x51, 0x49, 0xaa, 0x61, 0x2d, 0x7f,
      0xff, 0x13, 0x72, 0x20, 0x27, 0x5b, 0x16, 0x80,
  };
  const uint8_t plaintext[17] = {
      0x4b, 0x99, 0x23, 0x53, 0xa6, 0x6,  0x65, 0xbf, 0x99,
      0x2d, 0x3,  0x54, 0x82, 0xc1, 0xd2, 0x79, 0x0,
  };
  const uint8_t ciphertext_expected[17] = {
      0x2a, 0x40, 0x62, 0xd8, 0x35, 0xc5, 0x93, 0xea, 0x19,
      0xf8, 0x22, 0xad, 0x24, 0x24, 0x77, 0xd2, 0x80,
  };

  return lowmc_enc(&parameters_129_129_4, key, plaintext, ciphertext_expected);
}

static int LowMC_test_vector_129_129_4_3(void) {
  const uint8_t key[17] = {
      0xe7, 0x3a, 0xf2, 0x9c, 0xfc, 0x7a, 0xe5, 0x3e, 0x52,
      0x20, 0xd3, 0x1e, 0x2e, 0x59, 0x17, 0xda, 0x80,
  };
  const uint8_t plaintext[17] = {
      0x30, 0x4b, 0xa7, 0xa8, 0xde, 0x2b, 0x5c, 0xf8, 0x87,
      0xf9, 0xa4, 0x8a, 0xb7, 0x56, 0x1b, 0xf6, 0x80,
  };
  const uint8_t ciphertext_expected[17] = {
      0x5c, 0xd2, 0xc3, 0x55, 0x32, 0x8e, 0xfd, 0xe9, 0xf3,
      0x78, 0xc1, 0x61, 0x23, 0xd3, 0x3f, 0xb3, 0x0,
  };

  return lowmc_enc(&parameters_129_129_4, key, plaintext, ciphertext_expected);
}

static int LowMC_test_vector_129_129_4_4(void) {
  const uint8_t key[17] = {
      0x30, 0xf3, 0x34, 0x88, 0x53, 0x2d, 0x7e, 0xb8, 0xa5,
      0xf8, 0xfb, 0x4f, 0x2e, 0x63, 0xba, 0x56, 0x0,
  };
  const uint8_t plaintext[17] = {
      0xc2, 0x6a, 0x5d, 0xf9, 0x6,  0x15, 0x8d, 0xcb, 0x6a,
      0xc7, 0x89, 0x1d, 0xa9, 0xf4, 0x9f, 0x78, 0x0,
  };
  const uint8_t ciphertext_expected[17] = {
      0xb,  0x43, 0xb6, 0x5f, 0x7c, 0x53, 0x50, 0x6,  0xcf,
      0x27, 0xe8, 0x6f, 0x55, 0x1b, 0xd0, 0x15, 0x80,
  };

  return lowmc_enc(&parameters_129_129_4, key, plaintext, ciphertext_expected);
}
#endif

#if defined(WITH_LOWMC_192_192_4)
#include "../lowmc_192_192_4.h"
static const lowmc_parameters_t parameters_192_192_4 = lowmc_parameters_192_192_4;

static int LowMC_test_vector_192_192_4_1(void) {
  const uint8_t key[24] = {
      0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
      0x0,  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  };
  const uint8_t plaintext[24] = {
      0xab, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
      0x0,  0x0,  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  };
  const uint8_t ciphertext_expected[24] = {
      0xf8, 0xf7, 0xa2, 0x25, 0xde, 0x77, 0x12, 0x31, 0x29, 0x10, 0x7a, 0x20,
      0xf5, 0x54, 0x3a, 0xfa, 0x78, 0x33, 0x7,  0x66, 0x53, 0xba, 0x2b, 0x29,
  };
  return lowmc_enc(&parameters_192_192_4, key, plaintext, ciphertext_expected);
}

static int LowMC_test_vector_192_192_4_2(void) {
  const uint8_t key[24] = {
      0x81, 0xb8, 0x5d, 0xfe, 0x40, 0xf6, 0x12, 0x27, 0x5a, 0xa3, 0xf9, 0x19,
      0x91, 0x39, 0xeb, 0xaa, 0xe8, 0xdf, 0xf8, 0x36, 0x6f, 0x2d, 0xd3, 0x4e,
  };
  const uint8_t plaintext[24] = {
      0xb8, 0x65, 0xcc, 0xf3, 0xfc, 0xda, 0x8d, 0xdb, 0xed, 0x52, 0x7d, 0xc3,
      0x4d, 0xd4, 0x15, 0xd,  0x4a, 0x48, 0x2d, 0xcb, 0xf7, 0xe9, 0x64, 0x3c,
  };
  const uint8_t ciphertext_expected[24] = {
      0x95, 0xef, 0x9e, 0xd7, 0xc3, 0x78, 0x72, 0xa7, 0xb4, 0x60, 0x2a, 0x3f,
      0xa9, 0xc4, 0x6e, 0xbc, 0xb8, 0x42, 0x54, 0xed, 0xe,  0x44, 0xee, 0x9f,
  };
  return lowmc_enc(&parameters_192_192_4, key, plaintext, ciphertext_expected);
}

static int LowMC_test_vector_192_192_4_3(void) {
  const uint8_t key[24] = {
      0x24, 0x5,  0x97, 0x8f, 0xda, 0xad, 0x9b, 0x6d, 0x8d, 0xcd, 0xd1, 0x8a,
      0xc,  0x2c, 0xe,  0xc6, 0x8b, 0x69, 0xdd, 0xa,  0x37, 0x54, 0xfe, 0x38,
  };
  const uint8_t plaintext[24] = {
      0x33, 0xe8, 0xb4, 0x55, 0x2e, 0x95, 0xef, 0x52, 0x79, 0x49, 0x77, 0x6,
      0xbc, 0xe0, 0x1e, 0xcb, 0x4a, 0xcb, 0x86, 0x1,  0x41, 0xb7, 0xfc, 0x43,
  };
  const uint8_t ciphertext_expected[24] = {
      0xdd, 0xaf, 0xf,  0x9d, 0x9e, 0xdd, 0x57, 0x20, 0x69, 0xa8, 0x94, 0x9f,
      0xae, 0xa0, 0xd1, 0xfd, 0x2d, 0x91, 0xef, 0x26, 0x2b, 0x41, 0x1c, 0xaf,
  };
  return lowmc_enc(&parameters_192_192_4, key, plaintext, ciphertext_expected);
}

static int LowMC_test_vector_192_192_4_4(void) {
  const uint8_t key[24] = {
      0x56, 0x9d, 0x7d, 0x82, 0x23, 0x0,  0x94, 0x3d, 0x94, 0x83, 0x47, 0x74,
      0x27, 0xe8, 0x8e, 0xa2, 0x27, 0xa2, 0xe3, 0x17, 0x2c, 0x4,  0xbc, 0xd3,
  };
  const uint8_t plaintext[24] = {
      0xae, 0xeb, 0x9d, 0x5b, 0x61, 0xa2, 0xa5, 0x6d, 0xd5, 0x98, 0xf7, 0xda,
      0x26, 0xdf, 0xd7, 0x8c, 0xc9, 0x92, 0xe0, 0xae, 0xa3, 0xfc, 0x2e, 0x39,
  };
  const uint8_t ciphertext_expected[24] = {
      0x86, 0x98, 0x70, 0xae, 0x65, 0x47, 0xad, 0xa, 0xfe, 0xf2, 0x77, 0x93,
      0x17, 0xd,  0x96, 0xbc, 0x78, 0xe0, 0x40, 0x9, 0x69, 0x44, 0x80, 0x8f,
  };
  return lowmc_enc(&parameters_192_192_4, key, plaintext, ciphertext_expected);
}
#endif

#if defined(WITH_LOWMC_255_255_4)
#include "../lowmc_255_255_4.h"
static const lowmc_parameters_t parameters_255_255_4 = lowmc_parameters_255_255_4;

static int LowMC_test_vector_255_255_4_1(void) {
  const uint8_t key[32] = {
      0x80, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
      0x0,  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  };
  const uint8_t plaintext[32] = {
      0xab, 0xff, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
      0x0,  0x0,  0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
  };
  const uint8_t ciphertext_expected[32] = {
      0xd4, 0x72, 0x1d, 0x84, 0x6d, 0xd1, 0x4d, 0xba, 0x3a, 0x2c, 0x41,
      0x50, 0x1c, 0x2,  0xda, 0x28, 0x2e, 0xca, 0xfd, 0x72, 0xdf, 0x77,
      0x99, 0x2f, 0x39, 0x67, 0xef, 0xd6, 0xe8, 0xf3, 0xf3, 0x56,
  };

  return lowmc_enc(&parameters_255_255_4, key, plaintext, ciphertext_expected);
}

static int LowMC_test_vector_255_255_4_2(void) {
  const uint8_t key[32] = {
      0x7c, 0x20, 0xbe, 0x53, 0xb6, 0xd6, 0x0,  0x81, 0x49, 0xe1, 0x9a,
      0x34, 0xb9, 0x7d, 0x96, 0x84, 0xa0, 0x91, 0x4c, 0xaf, 0x9f, 0x7f,
      0x38, 0xb2, 0x49, 0x98, 0x11, 0x36, 0x9c, 0x3f, 0x53, 0xda,
  };
  const uint8_t plaintext[32] = {
      0x88, 0x63, 0xf1, 0x29, 0xc0, 0x38, 0x7a, 0xe5, 0xa4, 0x2,  0xa4,
      0x9b, 0xd6, 0x49, 0x27, 0xc4, 0xc6, 0x59, 0x64, 0xfb, 0x85, 0x31,
      0xb0, 0xd7, 0x61, 0xb1, 0x61, 0xb4, 0xc9, 0x7b, 0x75, 0x5e,
  };
  const uint8_t ciphertext_expected[32] = {
      0x3, 0xb6, 0xe4, 0xb6, 0x3c, 0xc8, 0xb0, 0x82, 0x68, 0xb6, 0x78, 0x1d, 0x5a, 0x62, 0x9d, 0x6e,
      0x3, 0x2,  0xc,  0x1c, 0x4,  0x8d, 0x46, 0x84, 0x16, 0x1b, 0x90, 0xad, 0x73, 0x33, 0x91, 0x26,
  };

  return lowmc_enc(&parameters_255_255_4, key, plaintext, ciphertext_expected);
}

static int LowMC_test_vector_255_255_4_3(void) {
  const uint8_t key[32] = {
      0x6d, 0xf9, 0xe7, 0x8d, 0xf,  0xc1, 0xb8, 0x70, 0xda, 0xbe, 0x52,
      0x5,  0x14, 0xb9, 0x59, 0x63, 0x6a, 0x42, 0x30, 0x4b, 0xf4, 0x3a,
      0x24, 0x8,  0x52, 0x45, 0x6,  0xc8, 0x1e, 0xa3, 0xb,  0x14,
  };
  const uint8_t plaintext[32] = {
      0x9e, 0x51, 0x78, 0x42, 0x5,  0x20, 0xb8, 0xcc, 0xa5, 0x29, 0x59,
      0x5b, 0x80, 0xc4, 0x70, 0x3b, 0x2d, 0xcf, 0x2a, 0x7,  0x30, 0x64,
      0x3a, 0x6f, 0x41, 0x27, 0x98, 0x60, 0x5f, 0x5,  0x2b, 0x68,
  };
  const uint8_t ciphertext_expected[32] = {
      0xf,  0x19, 0xfc, 0xc8, 0xbc, 0x18, 0x86, 0x9a, 0xab, 0x8e, 0x4f,
      0xe8, 0x1e, 0x97, 0x67, 0xd1, 0x8c, 0xfe, 0x71, 0x50, 0x81, 0x92,
      0x9f, 0x92, 0x96, 0x3b, 0x40, 0x0,  0x0,  0x6,  0x26, 0xf8,
  };

  return lowmc_enc(&parameters_255_255_4, key, plaintext, ciphertext_expected);
}

static int LowMC_test_vector_255_255_4_4(void) {
  const uint8_t key[32] = {
      0xb0, 0x71, 0xc6, 0xd4, 0xa3, 0x77, 0xe5, 0x51, 0x25, 0x4c, 0x5d,
      0xc4, 0x1,  0xa3, 0xd0, 0x8a, 0xcb, 0x99, 0x60, 0x9f, 0x41, 0x8a,
      0x8c, 0x22, 0x7,  0xf5, 0x12, 0x2b, 0x5a, 0x17, 0xfe, 0x9a,
  };
  const uint8_t plaintext[32] = {
      0xf7, 0x61, 0x6d, 0xc5, 0x14, 0xfd, 0xe,  0x10, 0x28, 0x56, 0x1d,
      0x9,  0x8a, 0xaf, 0xa5, 0x4c, 0x34, 0xbe, 0x72, 0x8c, 0xf2, 0x4a,
      0x50, 0x24, 0xdf, 0x17, 0xb9, 0xcc, 0x2e, 0x33, 0xfb, 0xfa,
  };
  const uint8_t ciphertext_expected[32] = {
      0x44, 0x48, 0xc7, 0xa, 0xc3, 0x86, 0x30, 0x21, 0xbe, 0x23, 0x2c, 0x63, 0x38, 0x16, 0x87, 0xcd,
      0x5d, 0xef, 0xb5, 0xb, 0xa2, 0x8d, 0x7b, 0x26, 0x8e, 0x19, 0x72, 0x7b, 0xae, 0xbc, 0x67, 0x9a,
  };

  return lowmc_enc(&parameters_255_255_4, key, plaintext, ciphertext_expected);
}
#endif

typedef int (*test_fn_t)(void);

static const test_fn_t tests[] = {
#if defined(WITH_LOWMC_128_128_20)
    LowMC_test_vector_128_128_20_1, LowMC_test_vector_128_128_20_2,
    LowMC_test_vector_128_128_20_3,
#endif
#if defined(WITH_LOWMC_192_192_30)
    LowMC_test_vector_192_192_30_1, LowMC_test_vector_192_192_30_2,
    LowMC_test_vector_192_192_30_3,
#endif
#if defined(WITH_LOWMC_256_256_38)
    LowMC_test_vector_256_256_38_1, LowMC_test_vector_256_256_38_2,
    LowMC_test_vector_256_256_38_3,
#endif
#if defined(WITH_LOWMC_129_129_4)
    LowMC_test_vector_129_129_4_1, LowMC_test_vector_129_129_4_2,
    LowMC_test_vector_129_129_4_3, LowMC_test_vector_129_129_4_4,
#endif
#if defined(WITH_LOWMC_192_192_4)
    LowMC_test_vector_192_192_4_1, LowMC_test_vector_192_192_4_2,
    LowMC_test_vector_192_192_4_3, LowMC_test_vector_192_192_4_4,
#endif
#if defined(WITH_LOWMC_255_255_4)
    LowMC_test_vector_255_255_4_1, LowMC_test_vector_255_255_4_2,
    LowMC_test_vector_255_255_4_3, LowMC_test_vector_255_255_4_4,
#endif
};

static const size_t num_tests = sizeof(tests) / sizeof(tests[0]);

int main(void) {
  int ret = 0;
  for (size_t s = 0; s < num_tests; ++s) {
    const int t = tests[s]();
    if (t) {
      printf("ERR: lowmc_enc " SIZET_FMT " FAILED (%d)\n", s, t);
      ret = -1;
    }
  }

  return ret;
}
