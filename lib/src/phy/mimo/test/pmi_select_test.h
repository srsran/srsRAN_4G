/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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

#define PMI_SELECT_TEST_NOF_CASES 16

typedef struct {
  cf_t h[2][2];
  float n;
  float snri_1l[4];
  float snri_2l[2];
  uint32_t pmi[2];
} pmi_select_test_case_gold_t;

static pmi_select_test_case_gold_t pmi_select_test_case_gold [PMI_SELECT_TEST_NOF_CASES] = {
  { /* Test case 1 */
    .h = {
      {+0.103430f+0.455920f*_Complex_I, +0.042050f+0.751883f*_Complex_I}, 
      {-0.641951f-0.053356f*_Complex_I, +0.217847f+0.504428f*_Complex_I}
      },
    .n = 0.181048, 
    .snri_1l = {5.793827f, 2.505115f, 3.350922f, 4.948020f},
    .snri_2l = {2.015732f, 1.848130f},
    .pmi = {0, 0},
  },
  { /* Test case 2 */
    .h = {
      {-0.957328f-0.624816f*_Complex_I, -0.741457f-0.657570f*_Complex_I}, 
      {+0.720104f+0.351137f*_Complex_I, +0.593419f-0.200211f*_Complex_I}
      },
    .n = 0.935527, 
    .snri_1l = {0.475069f, 3.077055f, 1.078656f, 2.473467f},
    .snri_2l = {0.747362f, 0.594324f},
    .pmi = {1, 0},
  },
  { /* Test case 3 */
    .h = {
      {-0.047530f-0.118039f*_Complex_I, -0.195528f-0.724032f*_Complex_I}, 
      {-0.619953f+0.960678f*_Complex_I, -0.325868f-0.120700f*_Complex_I}
      },
    .n = 0.803842, 
    .snri_1l = {1.331730f, 1.164592f, 1.660155f, 0.836167f},
    .snri_2l = {0.554942f, 0.579321f},
    .pmi = {2, 1},
  },
  { /* Test case 4 */
    .h = {
      {+0.635330f-0.751786f*_Complex_I, -0.536944f-0.185884f*_Complex_I}, 
      {+0.282517f-0.864615f*_Complex_I, -0.484380f-0.780479f*_Complex_I}
      },
    .n = 0.529556, 
    .snri_1l = {5.128973f, 0.465969f, 2.812367f, 2.782574f},
    .snri_2l = {1.381190f, 0.818813f},
    .pmi = {0, 0},
  },
  { /* Test case 5 */
    .h = {
      {-0.576996f+0.964470f*_Complex_I, -0.948065f+0.902764f*_Complex_I}, 
      {+0.988240f-0.056784f*_Complex_I, +0.489282f+0.975071f*_Complex_I}
      },
    .n = 0.852921, 
    .snri_1l = {2.772684f, 3.261802f, 5.698031f, 0.336455f},
    .snri_2l = {0.768370f, 1.469069f},
    .pmi = {2, 1},
  },
  { /* Test case 6 */
    .h = {
      {-0.381846f-0.998609f*_Complex_I, +0.903472f-0.393687f*_Complex_I}, 
      {-0.772703f-0.261637f*_Complex_I, -0.765452f-0.759318f*_Complex_I}
      },
    .n = 0.711912, 
    .snri_1l = {2.998736f, 2.538860f, 5.099274f, 0.438323f},
    .snri_2l = {0.809381f, 1.371548f},
    .pmi = {2, 1},
  },
  { /* Test case 7 */
    .h = {
      {+0.915028f-0.780771f*_Complex_I, -0.355424f+0.447925f*_Complex_I}, 
      {+0.577968f+0.765204f*_Complex_I, +0.342972f-0.999014f*_Complex_I}
      },
    .n = 0.101944, 
    .snri_1l = {12.424177f, 24.940449f, 5.411339f, 31.953288f},
    .snri_2l = {4.610588f, 7.664146f},
    .pmi = {3, 1},
  },
  { /* Test case 8 */
    .h = {
      {-0.259232f-0.654765f*_Complex_I, +0.829378f-0.793859f*_Complex_I}, 
      {+0.997978f+0.212295f*_Complex_I, -0.659012f-0.176220f*_Complex_I}
      },
    .n = 0.255783, 
    .snri_1l = {3.345813f, 9.635433f, 6.767844f, 6.213402f},
    .snri_2l = {3.215386f, 2.640976f},
    .pmi = {1, 0},
  },
  { /* Test case 9 */
    .h = {
      {-0.596630f+0.244853f*_Complex_I, -0.624622f+0.316537f*_Complex_I}, 
      {+0.767545f-0.645831f*_Complex_I, +0.262828f+0.251697f*_Complex_I}
      },
    .n = 0.876456, 
    .snri_1l = {0.367264f, 1.965908f, 1.215674f, 1.117498f},
    .snri_2l = {0.579923f, 0.479609f},
    .pmi = {1, 0},
  },
  { /* Test case 10 */
    .h = {
      {-0.643594f+0.172442f*_Complex_I, +0.291148f-0.026254f*_Complex_I}, 
      {+0.768244f+0.678173f*_Complex_I, -0.498968f-0.896649f*_Complex_I}
      },
    .n = 0.739473, 
    .snri_1l = {1.104856f, 2.455074f, 2.920106f, 0.639825f},
    .snri_2l = {0.557672f, 0.658911f},
    .pmi = {2, 1},
  },
  { /* Test case 11 */
    .h = {
      {+0.109032f-0.285542f*_Complex_I, -0.141055f+0.318945f*_Complex_I}, 
      {+0.559445f-0.211656f*_Complex_I, -0.206665f-0.643045f*_Complex_I}
      },
    .n = 0.054295, 
    .snri_1l = {8.472397f, 10.480333f, 4.074631f, 14.878099f},
    .snri_2l = {2.121444f, 2.979095f},
    .pmi = {3, 1},
  },
  { /* Test case 12 */
    .h = {
      {-0.505758f-0.710501f*_Complex_I, +0.803627f+0.023333f*_Complex_I}, 
      {+0.964886f+0.987055f*_Complex_I, -0.031782f+0.525138f*_Complex_I}
      },
    .n = 0.966024, 
    .snri_1l = {0.612742f, 3.102514f, 1.227107f, 2.488149f},
    .snri_2l = {0.848010f, 0.701000f},
    .pmi = {1, 0},
  },
  { /* Test case 13 */
    .h = {
      {+0.859761f-0.737655f*_Complex_I, -0.527019f+0.509133f*_Complex_I}, 
      {-0.804956f-0.303794f*_Complex_I, -0.180451f-0.100433f*_Complex_I}
      },
    .n = 0.199335, 
    .snri_1l = {4.402551f, 8.656756f, 10.092319f, 2.966987f},
    .snri_2l = {2.048224f, 2.462759f},
    .pmi = {2, 1},
  },
  { /* Test case 14 */
    .h = {
      {+0.473036f+0.227467f*_Complex_I, -0.593265f-0.308456f*_Complex_I}, 
      {+0.536321f+0.445264f*_Complex_I, -0.517440f-0.765554f*_Complex_I}
      },
    .n = 0.180788, 
    .snri_1l = {10.671400f, 0.736020f, 3.584109f, 7.823311f},
    .snri_2l = {2.029078f, 0.914443f},
    .pmi = {0, 0},
  },
  { /* Test case 15 */
    .h = {
      {-0.612271f+0.338114f*_Complex_I, -0.278903f+0.914426f*_Complex_I}, 
      {-0.191213f-0.136670f*_Complex_I, -0.548440f+0.607628f*_Complex_I}
      },
    .n = 0.798189, 
    .snri_1l = {2.309797f, 0.356735f, 0.731443f, 1.935089f},
    .snri_2l = {0.577612f, 0.490806f},
    .pmi = {0, 0},
  },
  { /* Test case 16 */
    .h = {
      {+0.990482f+0.513519f*_Complex_I, -0.576391f+0.922553f*_Complex_I}, 
      {-0.341962f+0.139785f*_Complex_I, +0.524684f+0.217012f*_Complex_I}
      },
    .n = 0.365092, 
    .snri_1l = {2.942635f, 4.964827f, 4.761949f, 3.145513f},
    .snri_2l = {1.291431f, 1.267123f},
    .pmi = {1, 0},
  },
};

#endif /* PMI_SELECT_TEST_H */
