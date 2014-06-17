/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The libLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the libLTE library.
 *
 * libLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * libLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */


#include <strings.h>

#include "lte/sync/sss.h"

/**
 * @brief Function documentation: initSSStables()
 * This function generates the scrambling sequences required for generation of
 * SSS sequence according with 3GPP TS 36.211 version 10.5.0 Release 10.
 */
void generate_zsc_tilde(int *z_tilde, int *s_tilde, int *c_tilde) {

  int i;
  int x[N_SSS];
  bzero(x, sizeof(int) * N_SSS);
  x[4] = 1;

  for (i = 0; i < 26; i++)
    x[i + 5] = (x[i + 2] + x[i]) % 2;
  for (i = 0; i < N_SSS; i++)
    s_tilde[i] = 1 - 2 * x[i];

  for (i = 0; i < 26; i++)
    x[i + 5] = (x[i + 3] + x[i]) % 2;
  for (i = 0; i < N_SSS; i++)
    c_tilde[i] = 1 - 2 * x[i];

  for (i = 0; i < 26; i++)
    x[i + 5] = (x[i + 4] + x[i + 2] + x[i + 1] + x[i]) % 2;
  for (i = 0; i < N_SSS; i++)
    z_tilde[i] = 1 - 2 * x[i];
}

void generate_m0m1(int N_id_1, int *m0, int *m1) {
  int q_prime = N_id_1 / (N_SSS - 1);
  int q = (N_id_1 + (q_prime * (q_prime + 1) / 2)) / (N_SSS - 1);
  int m_prime = N_id_1 + (q * (q + 1) / 2);
  *m0 = m_prime % N_SSS;
  *m1 = (*m0 + m_prime / N_SSS + 1) % N_SSS;
}


/* table[m0][m1-1]=N_id_1 */
void generate_N_id_1_table(int table[30][30]) {
  int m0, m1;
  int N_id_1;
  for (N_id_1=0;N_id_1<168;N_id_1++) {
    generate_m0m1(N_id_1, &m0, &m1);
    table[m0][m1-1] = N_id_1;
  }
}


void generate_s(int *s, int *s_tilde, int m0_m1) {
  int i;
  for (i = 0; i < N_SSS; i++) {
    s[i] = s_tilde[(i + m0_m1) % N_SSS];
  }
}

void generate_s_all(int s[N_SSS][N_SSS], int *s_tilde) {
  int i;
  for (i = 0; i < N_SSS; i++) {
    generate_s(s[i], s_tilde, i);
  }
}

void generate_c(int *c, int *c_tilde, int N_id_2, int is_c0) {
  int i;
  for (i = 0; i < N_SSS; i++) {
    c[i] = c_tilde[(i + N_id_2 + (is_c0 > 0 ? 3 : 0)) % N_SSS];
  }
}

void generate_z(int *z, int *z_tilde, int m0_m1) {
  int i;
  for (i = 0; i < N_SSS; i++) {
    z[i] = z_tilde[(i + (m0_m1 % 8)) % N_SSS];
  }
}

void generate_z_all(int z[N_SSS][N_SSS], int *z_tilde) {
  int i;
  for (i = 0; i < N_SSS; i++) {
    generate_z(z[i], z_tilde, i);
  }
}

void generate_sss_all_tables(struct sss_tables *tables, int N_id_2) {
  int i;
  int s_t[N_SSS], c_t[N_SSS], z_t[N_SSS];

  generate_zsc_tilde(z_t, s_t, c_t);
  generate_s_all(tables->s, s_t);
  generate_z_all(tables->z1, z_t);
  for (i = 0; i < 2; i++) {
    generate_c(tables->c[i], c_t, N_id_2, i);
  }
  tables->N_id_2 = N_id_2;
}

void sss_generate(float *signal0, float *signal5, int cell_id) {

  int i;
  int id1 = cell_id / 3;
  int id2 = cell_id % 3;
  int m0;
  int m1;
  int s_t[N_SSS], c_t[N_SSS], z_t[N_SSS];
  int s0[N_SSS], s1[N_SSS], c0[N_SSS], c1[N_SSS], z1_0[N_SSS], z1_1[N_SSS];

  generate_m0m1(id1, &m0, &m1);
  generate_zsc_tilde(z_t, s_t, c_t);

  generate_s(s0, s_t, m0);
  generate_s(s1, s_t, m1);

  generate_c(c0, c_t, id2, 0);
  generate_c(c1, c_t, id2, 1);

  generate_z(z1_0, z_t, m0);
  generate_z(z1_1, z_t, m1);

  for (i = 0; i < N_SSS; i++) {
    /** Even Resource Elements: Sub-frame 0*/
    signal0[2 * i] = (float) (s0[i] * c0[i]);
    /** Odd Resource Elements: Sub-frame 0*/
    signal0[2 * i + 1] = (float) (s1[i] * c1[i] * z1_0[i]);
  }
  for (i = 0; i < N_SSS; i++) {
    /** Even Resource Elements: Sub-frame 5*/
    signal5[2 * i] = (float) (s1[i] * c0[i]);
    /** Odd Resource Elements: Sub-frame 5*/
    signal5[2 * i + 1] = (float) (s0[i] * c1[i] * z1_1[i]);
  }
}

