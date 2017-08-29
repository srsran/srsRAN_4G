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
  cf_t h[2][2];     /* Channel estimate */
  float n;          /* Noise estimation */
  float snri_1l[4]; /* SINR Approximation for 1 layer (linear) */
  float snri_2l[2]; /* SINR Approximation for 2 layers (linear) */
  uint32_t pmi[2];  /* Precoding Matrix Indicator for 1 and 2 layers */
  uint32_t ri;      /* Rank indicator */
  float k;          /* Condition number (κ) in dB */
} pmi_select_test_case_gold_t;

static pmi_select_test_case_gold_t pmi_select_test_case_gold [PMI_SELECT_TEST_NOF_CASES] = {
  { /* Test case 1 */
    .h = {
      {+0.861552f+0.205546f*_Complex_I, +0.955538f-0.052364f*_Complex_I}, 
      {-0.812807f-0.287487f*_Complex_I, +0.323470f-0.048843f*_Complex_I}
      },
    .n = 0.671022, 
    .snri_1l = {1.233339f, 2.567786f, 2.065012f, 1.736113f},
    .snri_2l = {0.893739f, 0.841600f},
    .pmi = {1, 0},
    .ri = 2,
    .k = 4.6758,
  },
  { /* Test case 2 */
    .h = {
      {+0.919290f+0.824394f*_Complex_I, -0.821831f-0.797741f*_Complex_I}, 
      {+0.595485f-0.413411f*_Complex_I, +0.181551f-0.896824f*_Complex_I}
      },
    .n = 0.504128, 
    .snri_1l = {5.697916f, 2.631874f, 4.143253f, 4.186538f},
    .snri_2l = {1.897704f, 1.718554f},
    .pmi = {0, 0},
    .ri = 2,
    .k = 4.8715,
  },
  { /* Test case 3 */
    .h = {
      {+0.536752f-0.093497f*_Complex_I, -0.434029f+0.474769f*_Complex_I}, 
      {-0.549280f+0.019771f*_Complex_I, -0.337421f-0.234972f*_Complex_I}
      },
    .n = 0.905483, 
    .snri_1l = {0.363469f, 0.941684f, 0.408025f, 0.897129f},
    .snri_2l = {0.311544f, 0.307074f},
    .pmi = {1, 0},
    .ri = 1,
    .k = 6.2219,
  },
  { /* Test case 4 */
    .h = {
      {+0.930515f-0.233960f*_Complex_I, +0.256535f+0.982387f*_Complex_I}, 
      {-0.735938f-0.426345f*_Complex_I, +0.236604f+0.412383f*_Complex_I}
      },
    .n = 0.535206, 
    .snri_1l = {2.487298f, 2.932872f, 4.009672f, 1.410498f},
    .snri_2l = {1.082110f, 1.248639f},
    .pmi = {2, 1},
    .ri = 2,
    .k = 5.9758,
  },
  { /* Test case 5 */
    .h = {
      {-0.613578f-0.908683f*_Complex_I, +0.378870f+0.770083f*_Complex_I}, 
      {-0.899090f+0.679589f*_Complex_I, -0.631132f-0.763690f*_Complex_I}
      },
    .n = 0.410415, 
    .snri_1l = {2.929140f, 7.281276f, 7.632612f, 2.577804f},
    .snri_2l = {2.045272f, 2.159393f},
    .pmi = {2, 1},
    .ri = 2,
    .k = 6.8485,
  },
  { /* Test case 6 */
    .h = {
      {-0.759543f+0.979731f*_Complex_I, +0.144185f-0.300384f*_Complex_I}, 
      {+0.898780f-0.582955f*_Complex_I, -0.487230f+0.331654f*_Complex_I}
      },
    .n = 0.973345, 
    .snri_1l = {0.151784f, 3.077111f, 2.165454f, 1.063441f},
    .snri_2l = {0.755981f, 0.503361f},
    .pmi = {1, 0},
    .ri = 1,
    .k = 18.0800,
  },
  { /* Test case 7 */
    .h = {
      {+0.245400f-0.537444f*_Complex_I, -0.872924f-0.895583f*_Complex_I}, 
      {-0.252981f+0.803513f*_Complex_I, -0.667497f+0.586583f*_Complex_I}
      },
    .n = 0.373014, 
    .snri_1l = {3.403660f, 5.744504f, 7.385295f, 1.762868f},
    .snri_2l = {1.642126f, 2.130892f},
    .pmi = {2, 1},
    .ri = 2,
    .k = 7.1484,
  },
  { /* Test case 8 */
    .h = {
      {+0.664109f-0.281444f*_Complex_I, +0.507669f-0.822295f*_Complex_I}, 
      {+0.243726f-0.316646f*_Complex_I, -0.211814f+0.097341f*_Complex_I}
      },
    .n = 0.460547, 
    .snri_1l = {1.948731f, 1.673386f, 2.389606f, 1.232511f},
    .snri_2l = {0.623422f, 0.659548f},
    .pmi = {2, 1},
    .ri = 1,
    .k = 9.8582,
  },
  { /* Test case 9 */
    .h = {
      {+0.290905f-0.072573f*_Complex_I, +0.027042f+0.179635f*_Complex_I}, 
      {+0.628853f-0.625656f*_Complex_I, -0.805634f+0.222660f*_Complex_I}
      },
    .n = 0.051942, 
    .snri_1l = {20.229536f, 10.736104f, 15.206118f, 15.759523f},
    .snri_2l = {2.426844f, 2.175025f},
    .pmi = {0, 0},
    .ri = 1,
    .k = 12.8396,
  },
  { /* Test case 10 */
    .h = {
      {+0.151454f-0.701886f*_Complex_I, +0.684689f-0.943441f*_Complex_I}, 
      {-0.000548f+0.513340f*_Complex_I, -0.121950f+0.592212f*_Complex_I}
      },
    .n = 0.293556, 
    .snri_1l = {0.848833f, 7.679596f, 3.011330f, 5.517099f},
    .snri_2l = {1.442768f, 0.788147f},
    .pmi = {1, 0},
    .ri = 1,
    .k = 22.0345,
  },
  { /* Test case 11 */
    .h = {
      {-0.769587f+0.330477f*_Complex_I, -0.249817f+0.920280f*_Complex_I}, 
      {+0.657787f+0.886236f*_Complex_I, +0.683553f-0.774601f*_Complex_I}
      },
    .n = 0.648287, 
    .snri_1l = {1.312873f, 4.697041f, 5.064183f, 0.945731f},
    .snri_2l = {0.993660f, 1.125611f},
    .pmi = {2, 1},
    .ri = 1,
    .k = 12.9593,
  },
  { /* Test case 12 */
    .h = {
      {-0.038392f+0.542607f*_Complex_I, -0.866959f-0.879276f*_Complex_I}, 
      {+0.795542f-0.475085f*_Complex_I, -0.005540f+0.302139f*_Complex_I}
      },
    .n = 0.133604, 
    .snri_1l = {6.257962f, 14.479088f, 15.459994f, 5.277056f},
    .snri_2l = {3.523645f, 3.845011f},
    .pmi = {2, 1},
    .ri = 1,
    .k = 7.6196,
  },
  { /* Test case 13 */
    .h = {
      {+0.277091f-0.237022f*_Complex_I, -0.230114f-0.399963f*_Complex_I}, 
      {+0.531396f-0.319721f*_Complex_I, +0.305831f+0.837853f*_Complex_I}
      },
    .n = 0.456267, 
    .snri_1l = {1.272387f, 2.072182f, 1.744873f, 1.599696f},
    .snri_2l = {0.720254f, 0.700519f},
    .pmi = {1, 0},
    .ri = 2,
    .k = 5.9972,
  },
  { /* Test case 14 */
    .h = {
      {-0.115006f+0.764806f*_Complex_I, -0.091628f-0.960249f*_Complex_I}, 
      {+0.890564f-0.316470f*_Complex_I, -0.561762f+0.532055f*_Complex_I}
      },
    .n = 0.342804, 
    .snri_1l = {2.060602f, 6.750694f, 8.002153f, 0.809143f},
    .snri_2l = {1.036665f, 1.575640f},
    .pmi = {2, 1},
    .ri = 1,
    .k = 18.9097,
  },
  { /* Test case 15 */
    .h = {
      {+0.237613f+0.203137f*_Complex_I, -0.093958f+0.298835f*_Complex_I}, 
      {-0.979675f-0.314559f*_Complex_I, +0.198162f-0.013401f*_Complex_I}
      },
    .n = 0.701774, 
    .snri_1l = {0.466961f, 1.376956f, 0.827475f, 1.016442f},
    .snri_2l = {0.386935f, 0.354722f},
    .pmi = {1, 0},
    .ri = 1,
    .k = 11.2469,
  },
  { /* Test case 16 */
    .h = {
      {+0.775605f+0.528142f*_Complex_I, -0.889884f+0.975918f*_Complex_I}, 
      {-0.803276f-0.749350f*_Complex_I, +0.299566f-0.271046f*_Complex_I}
      },
    .n = 0.676230, 
    .snri_1l = {0.661772f, 5.245671f, 3.261470f, 2.645973f},
    .snri_2l = {1.354145f, 0.857372f},
    .pmi = {1, 0},
    .ri = 1,
    .k = 10.7137,
  },
};

#endif /* PMI_SELECT_TEST_H */
