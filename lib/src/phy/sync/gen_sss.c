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

#include <strings.h>

#include "srslte/phy/sync/sss.h"

/**
 * @brief Function documentation: initSSStables()
 * This function generates the scrambling sequences required for generation of
 * SSS sequence according with 3GPP TS 36.211 version 10.5.0 Release 10.
 */
void generate_zsc_tilde(int* z_tilde, int* s_tilde, int* c_tilde)
{

  int i;
  int x[SRSLTE_SSS_N];
  bzero(x, sizeof(int) * SRSLTE_SSS_N);
  x[4] = 1;

  for (i = 0; i < 26; i++)
    x[i + 5] = (x[i + 2] + x[i]) % 2;
  for (i = 0; i < SRSLTE_SSS_N; i++)
    s_tilde[i] = 1 - 2 * x[i];

  for (i = 0; i < 26; i++)
    x[i + 5] = (x[i + 3] + x[i]) % 2;
  for (i = 0; i < SRSLTE_SSS_N; i++)
    c_tilde[i] = 1 - 2 * x[i];

  for (i = 0; i < 26; i++)
    x[i + 5] = (x[i + 4] + x[i + 2] + x[i + 1] + x[i]) % 2;
  for (i = 0; i < SRSLTE_SSS_N; i++)
    z_tilde[i] = 1 - 2 * x[i];
}

void generate_m0m1(uint32_t N_id_1, uint32_t* m0, uint32_t* m1)
{
  uint32_t q_prime = N_id_1 / (SRSLTE_SSS_N - 1);
  uint32_t q       = (N_id_1 + (q_prime * (q_prime + 1) / 2)) / (SRSLTE_SSS_N - 1);
  uint32_t m_prime = N_id_1 + (q * (q + 1) / 2);
  *m0              = m_prime % SRSLTE_SSS_N;
  *m1              = (*m0 + m_prime / SRSLTE_SSS_N + 1) % SRSLTE_SSS_N;
}

/* table[m0][m1-1]=N_id_1 */
void generate_N_id_1_table(uint32_t table[30][30])
{
  uint32_t m0, m1;
  uint32_t N_id_1;
  for (N_id_1 = 0; N_id_1 < 168; N_id_1++) {
    generate_m0m1(N_id_1, &m0, &m1);
    table[m0][m1 - 1] = N_id_1;
  }
}

void generate_s(int* s, int* s_tilde, uint32_t m0_m1)
{
  uint32_t i;
  for (i = 0; i < SRSLTE_SSS_N; i++) {
    s[i] = s_tilde[(i + m0_m1) % SRSLTE_SSS_N];
  }
}

void generate_s_all(int s[SRSLTE_SSS_N][SRSLTE_SSS_N], int* s_tilde)
{
  uint32_t i;
  for (i = 0; i < SRSLTE_SSS_N; i++) {
    generate_s(s[i], s_tilde, i);
  }
}

void generate_c(int* c, int* c_tilde, uint32_t N_id_2, bool is_c0)
{
  uint32_t i;
  for (i = 0; i < SRSLTE_SSS_N; i++) {
    c[i] = c_tilde[(i + N_id_2 + (is_c0 ? 3 : 0)) % SRSLTE_SSS_N];
  }
}

void generate_z(int* z, int* z_tilde, uint32_t m0_m1)
{
  uint32_t i;
  for (i = 0; i < SRSLTE_SSS_N; i++) {
    z[i] = z_tilde[(i + (m0_m1 % 8)) % SRSLTE_SSS_N];
  }
}

void generate_z_all(int z[SRSLTE_SSS_N][SRSLTE_SSS_N], int* z_tilde)
{
  uint32_t i;
  for (i = 0; i < SRSLTE_SSS_N; i++) {
    generate_z(z[i], z_tilde, i);
  }
}

void generate_sss_all_tables(srslte_sss_tables_t* tables, uint32_t N_id_2)
{
  uint32_t i;
  int      s_t[SRSLTE_SSS_N], c_t[SRSLTE_SSS_N], z_t[SRSLTE_SSS_N];

  generate_zsc_tilde(z_t, s_t, c_t);
  generate_s_all(tables->s, s_t);
  generate_z_all(tables->z1, z_t);
  for (i = 0; i < 2; i++) {
    generate_c(tables->c[i], c_t, N_id_2, i != 0);
  }
}

void srslte_sss_generate(float* signal0, float* signal5, uint32_t cell_id)
{

  uint32_t i;
  uint32_t id1 = cell_id / 3;
  uint32_t id2 = cell_id % 3;
  uint32_t m0;
  uint32_t m1;
  int      s_t[SRSLTE_SSS_N], c_t[SRSLTE_SSS_N], z_t[SRSLTE_SSS_N];
  int s0[SRSLTE_SSS_N], s1[SRSLTE_SSS_N], c0[SRSLTE_SSS_N], c1[SRSLTE_SSS_N], z1_0[SRSLTE_SSS_N], z1_1[SRSLTE_SSS_N];

  generate_m0m1(id1, &m0, &m1);
  generate_zsc_tilde(z_t, s_t, c_t);

  generate_s(s0, s_t, m0);
  generate_s(s1, s_t, m1);

  generate_c(c0, c_t, id2, 0);
  generate_c(c1, c_t, id2, 1);

  generate_z(z1_0, z_t, m0);
  generate_z(z1_1, z_t, m1);

  for (i = 0; i < SRSLTE_SSS_N; i++) {
    /** Even Resource Elements: Sub-frame 0*/
    signal0[2 * i] = (float)(s0[i] * c0[i]);
    /** Odd Resource Elements: Sub-frame 0*/
    signal0[2 * i + 1] = (float)(s1[i] * c1[i] * z1_0[i]);
  }
  for (i = 0; i < SRSLTE_SSS_N; i++) {
    /** Even Resource Elements: Sub-frame 5*/
    signal5[2 * i] = (float)(s1[i] * c0[i]);
    /** Odd Resource Elements: Sub-frame 5*/
    signal5[2 * i + 1] = (float)(s0[i] * c1[i] * z1_1[i]);
  }
}
