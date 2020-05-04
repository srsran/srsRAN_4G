/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef PMI_SELECT_TEST_H
#define PMI_SELECT_TEST_H

#define PMI_SELECT_TEST_NOF_CASES 20

#include <inttypes.h>
#include "srslte/config.h"

typedef struct {
  cf_t     h[2][2];    /* Channel estimate */
  float    n;          /* Noise estimation */
  float    snri_1l[4]; /* SINR Approximation for 1 layer (linear) */
  float    snri_2l[2]; /* SINR Approximation for 2 layers (linear) */
  uint32_t pmi[2];     /* Precoding Matrix Indicator for 1 and 2 layers */
  uint32_t ri;         /* Rank indicator */
  float    k;          /* Condition number (κ) in dB */
} pmi_select_test_case_gold_t;

static pmi_select_test_case_gold_t pmi_select_test_case_gold[PMI_SELECT_TEST_NOF_CASES] = {
    {
        /* Test case 1 */
        .h       = {{+0.626226f + 0.060103f * _Complex_I, -0.233387f - 0.449860f * _Complex_I},
              {+0.234558f - 0.502742f * _Complex_I, +0.150990f - 0.096722f * _Complex_I}},
        .n       = 0.227713,
        .snri_1l = {2.728043f, 1.630673f, 3.226421f, 1.132295f},
        .snri_2l = {1.797660f, 1.982149f},
        .pmi     = {2, 1},
        .ri      = 1,
        .k       = 6.4007,
    },
    {
        /* Test case 2 */
        .h       = {{+0.608899f - 0.825846f * _Complex_I, +0.972208f + 0.604183f * _Complex_I},
              {-0.940016f + 0.978290f * _Complex_I, +0.071328f - 0.866107f * _Complex_I}},
        .n       = 0.939398,
        .snri_1l = {0.686850f, 4.591972f, 3.773925f, 1.504897f},
        .snri_2l = {2.298235f, 1.761859f},
        .pmi     = {1, 0},
        .ri      = 1,
        .k       = 11.1305,
    },
    {
        /* Test case 3 */
        .h       = {{-0.963645f + 0.770719f * _Complex_I, +0.367677f + 0.798010f * _Complex_I},
              {+0.567473f + 0.251875f * _Complex_I, +0.068275f - 0.724262f * _Complex_I}},
        .n       = 0.217802,
        .snri_1l = {3.209674f, 11.525338f, 11.962786f, 2.772226f},
        .snri_2l = {3.226053f, 3.526363f},
        .pmi     = {2, 1},
        .ri      = 1,
        .k       = 15.4589,
    },
    {
        /* Test case 4 */
        .h       = {{-0.635718f + 0.879322f * _Complex_I, -0.916360f - 0.291089f * _Complex_I},
              {-0.786117f - 0.178742f * _Complex_I, +0.232887f + 0.968699f * _Complex_I}},
        .n       = 0.945579,
        .snri_1l = {1.818313f, 2.141519f, 1.995787f, 1.964045f},
        .snri_2l = {1.965011f, 1.958537f},
        .pmi     = {1, 0},
        .ri      = 2,
        .k       = 1.2910,
    },
    {
        /* Test case 5 */
        .h       = {{+0.353289f + 0.324764f * _Complex_I, +0.976605f - 0.511669f * _Complex_I},
              {+0.533663f - 0.408985f * _Complex_I, -0.326601f + 0.360357f * _Complex_I}},
        .n       = 0.527847,
        .snri_1l = {1.173803f, 2.869865f, 2.273783f, 1.769885f},
        .snri_2l = {1.871430f, 1.713879f},
        .pmi     = {1, 0},
        .ri      = 2,
        .k       = 5.5388,
    },
    {
        /* Test case 6 */
        .h       = {{-0.176813f + 0.103585f * _Complex_I, +0.205276f + 0.167141f * _Complex_I},
              {+0.501040f + 0.023640f * _Complex_I, +0.167066f - 0.834815f * _Complex_I}},
        .n       = 0.719570,
        .snri_1l = {0.490387f, 1.022313f, 1.111245f, 0.401456f},
        .snri_2l = {0.578124f, 0.597176f},
        .pmi     = {2, 1},
        .ri      = 1,
        .k       = 21.8808,
    },
    {
        /* Test case 7 */
        .h       = {{+0.992312f + 0.773088f * _Complex_I, -0.290931f - 0.090610f * _Complex_I},
              {+0.942518f - 0.173145f * _Complex_I, -0.307102f - 0.564536f * _Complex_I}},
        .n       = 0.125655,
        .snri_1l = {19.459529f, 4.467420f, 18.044021f, 5.882928f},
        .snri_2l = {8.055238f, 6.832247f},
        .pmi     = {0, 0},
        .ri      = 1,
        .k       = 9.9136,
    },
    {
        /* Test case 8 */
        .h       = {{-0.382171f - 0.980395f * _Complex_I, +0.452209f + 0.686427f * _Complex_I},
              {+0.565744f + 0.844664f * _Complex_I, +0.387575f + 0.541908f * _Complex_I}},
        .n       = 0.042660,
        .snri_1l = {26.560881f, 49.864772f, 33.269985f, 43.155668f},
        .snri_2l = {37.201526f, 34.461078f},
        .pmi     = {1, 0},
        .ri      = 2,
        .k       = 3.1172,
    },
    {
        /* Test case 9 */
        .h       = {{-0.243628f - 0.461891f * _Complex_I, +0.408679f + 0.346062f * _Complex_I},
              {+0.459026f - 0.045016f * _Complex_I, -0.551446f + 0.247433f * _Complex_I}},
        .n       = 0.236445,
        .snri_1l = {1.429443f, 3.381496f, 0.227617f, 4.583322f},
        .snri_2l = {1.272903f, 2.118832f},
        .pmi     = {3, 1},
        .ri      = 1,
        .k       = 24.1136,
    },
    {
        /* Test case 10 */
        .h       = {{-0.645752f - 0.784222f * _Complex_I, +0.659287f - 0.635545f * _Complex_I},
              {+0.533843f - 0.801809f * _Complex_I, +0.868957f - 0.020472f * _Complex_I}},
        .n       = 0.193245,
        .snri_1l = {13.697372f, 4.693597f, 1.561737f, 16.829232f},
        .snri_2l = {2.961344f, 5.773049f},
        .pmi     = {3, 1},
        .ri      = 1,
        .k       = 17.5194,
    },
    {
        /* Test case 11 */
        .h       = {{+0.791783f + 0.544990f * _Complex_I, -0.801821f - 0.376120f * _Complex_I},
              {-0.911669f - 0.642035f * _Complex_I, +0.114590f - 0.322089f * _Complex_I}},
        .n       = 0.210146,
        .snri_1l = {2.340213f, 12.261749f, 5.921675f, 8.680286f},
        .snri_2l = {6.912040f, 4.520201f},
        .pmi     = {1, 0},
        .ri      = 2,
        .k       = 7.7819,
    },
    {
        /* Test case 12 */
        .h       = {{+0.020305f - 0.218290f * _Complex_I, +0.812729f - 0.890767f * _Complex_I},
              {+0.257848f + 0.002566f * _Complex_I, -0.796932f - 0.136558f * _Complex_I}},
        .n       = 0.997560,
        .snri_1l = {0.591218f, 1.636514f, 1.880263f, 0.347469f},
        .snri_2l = {0.869026f, 0.967991f},
        .pmi     = {2, 1},
        .ri      = 1,
        .k       = 12.9774,
    },
    {
        /* Test case 13 */
        .h       = {{+0.623205f - 0.219990f * _Complex_I, -0.028697f + 0.854712f * _Complex_I},
              {+0.788896f + 0.834988f * _Complex_I, -0.724907f + 0.427148f * _Complex_I}},
        .n       = 0.618337,
        .snri_1l = {3.706176f, 1.461946f, 0.479632f, 4.688490f},
        .snri_2l = {1.444336f, 2.102567f},
        .pmi     = {3, 1},
        .ri      = 1,
        .k       = 17.0493,
    },
    {
        /* Test case 14 */
        .h       = {{-0.313424f + 0.292955f * _Complex_I, +0.872055f + 0.666304f * _Complex_I},
              {-0.750452f - 0.203436f * _Complex_I, +0.461171f + 0.499644f * _Complex_I}},
        .n       = 0.835221,
        .snri_1l = {2.560265f, 0.379539f, 0.976562f, 1.963242f},
        .snri_2l = {1.380223f, 1.109300f},
        .pmi     = {0, 0},
        .ri      = 1,
        .k       = 10.1729,
    },
    {
        /* Test case 15 */
        .h       = {{-0.355079f - 0.339153f * _Complex_I, +0.104523f + 0.238943f * _Complex_I},
              {+0.958258f - 0.278727f * _Complex_I, +0.098617f + 0.513019f * _Complex_I}},
        .n       = 0.413901,
        .snri_1l = {1.633620f, 2.178855f, 0.809297f, 3.003178f},
        .snri_2l = {1.250898f, 1.512017f},
        .pmi     = {3, 1},
        .ri      = 1,
        .k       = 10.8925,
    },
    {
        /* Test case 16 */
        .h       = {{-0.015310f + 0.675606f * _Complex_I, +0.389486f + 0.478144f * _Complex_I},
              {+0.945468f + 0.908349f * _Complex_I, -0.344490f - 0.936155f * _Complex_I}},
        .n       = 0.356869,
        .snri_1l = {5.024121f, 4.926495f, 7.364348f, 2.586268f},
        .snri_2l = {3.165416f, 3.851590f},
        .pmi     = {2, 1},
        .ri      = 2,
        .k       = 7.7799,
    },
    {
        /* Test case 17 */
        .h       = {{1.0f, 0.0f}, {0.0f, 0.0f}},
        .n       = 0.0f,
        .snri_1l = {5e8f, 5e8f, 5e8f, 5e8f},
        .snri_2l = {0.0f, 0.0f},
        .pmi     = {0, 0},
        .ri      = 2,
        .k       = 93.0,
    },
    {
        /* Test case 18 */
        .h       = {{1.0f, 0.0f}, {1.0f, 0.0f}},
        .n       = 0.0f,
        .snri_1l = {2e9f, 0.0f, 1e9f, 1e9f},
        .snri_2l = {1e9f, 0.0f},
        .pmi     = {0, 0},
        .ri      = 2,
        .k       = 96.0,
    },
    {
        /* Test case 19 */
        .h       = {{1.0f, 1.0f}, {1.0f, 1.0f}},
        .n       = 0.0f,
        .snri_1l = {4e9f, 0.0f, 2e9f, 2e9f},
        .snri_2l = {2e9f, 0.0f},
        .pmi     = {0, 0},
        .ri      = 1,
        .k       = 99.0,
    },
    {
        /* Test case 20 */
        .h       = {{1.0f, 0.0f}, {0.0f, 1.0f}},
        .n       = 0.0f,
        .snri_1l = {1e9f, 1e9f, 1e9f, 1e9f},
        .snri_2l = {1e9f, 1e9f},
        .pmi     = {0, 0},
        .ri      = 2,
        .k       = 0.0,
    },
};

#endif /* PMI_SELECT_TEST_H */
