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

#include "srslte/phy/utils/random.h"
#include <complex>
#include <random>

class random_wrap
{
private:
  std::mt19937* mt19937;

public:
  explicit random_wrap(uint32_t seed) { mt19937 = new std::mt19937(seed); }

  ~random_wrap() { delete mt19937; }

  float uniform_real_dist(float min, float max)
  {
    std::uniform_real_distribution<float> dist(min, max);
    return dist(*mt19937);
  }

  int uniform_int_dist(int min, int max)
  {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(*mt19937);
  }

  float gauss_dist(float sigma)
  {
    std::normal_distribution<float> dist(sigma);
    return dist(*mt19937);
  }
};

extern "C" {

srslte_random_t srslte_random_init(uint32_t seed)
{
  return (srslte_random_t)(new random_wrap(seed));
}

int srslte_random_uniform_int_dist(srslte_random_t q, int min, int max)
{
  int ret = 0;

  if (q) {
    auto* h = (random_wrap*)q;
    ret     = h->uniform_int_dist(min, max);
  }

  return ret;
}

float srslte_random_uniform_real_dist(srslte_random_t q, float min, float max)
{
  float ret = NAN;

  if (q) {
    auto* h = (random_wrap*)q;
    while (std::isnan(ret)) {
      ret = h->uniform_real_dist(min, max);
    }
  }

  return ret;
}

cf_t srslte_random_uniform_complex_dist(srslte_random_t q, float min, float max)
{
  cf_t ret = NAN;

  if (q) {
    auto*               h = (random_wrap*)q;
    std::complex<float> x = std::complex<float>(h->uniform_real_dist(min, max), h->uniform_real_dist(min, max));
    ret                   = *((cf_t*)&x);
  }

  return ret;
}

void srslte_random_uniform_complex_dist_vector(srslte_random_t q, cf_t* vector, uint32_t nsamples, float min, float max)
{
  for (uint32_t i = 0; i < nsamples; i++) {
    vector[i] = srslte_random_uniform_complex_dist(q, min, max);
  }
}

float srslte_random_gauss_dist(srslte_random_t q, float std_dev)
{
  float ret = NAN;

  if (q) {
    auto* h = (random_wrap*)q;
    ret     = h->gauss_dist(std_dev);
  }

  return ret;
}

bool srslte_random_bool(srslte_random_t q, float prob_true)
{
  return srslte_random_uniform_real_dist(q, 0, 1) < prob_true;
}

void srslte_random_free(srslte_random_t q)
{
  if (q) {
    delete (random_wrap*)q;
  }
}
}
