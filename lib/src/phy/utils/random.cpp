/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/phy/utils/random.h"
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

srsran_random_t srsran_random_init(uint32_t seed)
{
  return (srsran_random_t)(new random_wrap(seed));
}

int srsran_random_uniform_int_dist(srsran_random_t q, int min, int max)
{
  int ret = 0;

  if (q) {
    auto* h = (random_wrap*)q;
    ret     = h->uniform_int_dist(min, max);
  }

  return ret;
}

float srsran_random_uniform_real_dist(srsran_random_t q, float min, float max)
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

cf_t srsran_random_uniform_complex_dist(srsran_random_t q, float min, float max)
{
  cf_t ret = NAN;

  if (q) {
    auto*               h = (random_wrap*)q;
    std::complex<float> x = std::complex<float>(h->uniform_real_dist(min, max), h->uniform_real_dist(min, max));
    ret                   = *((cf_t*)&x);
  }

  return ret;
}

void srsran_random_uniform_complex_dist_vector(srsran_random_t q, cf_t* vector, uint32_t nsamples, float min, float max)
{
  for (uint32_t i = 0; i < nsamples; i++) {
    vector[i] = srsran_random_uniform_complex_dist(q, min, max);
  }
}

float srsran_random_gauss_dist(srsran_random_t q, float std_dev)
{
  float ret = NAN;

  if (q) {
    auto* h = (random_wrap*)q;
    ret     = h->gauss_dist(std_dev);
  }

  return ret;
}

bool srsran_random_bool(srsran_random_t q, float prob_true)
{
  return srsran_random_uniform_real_dist(q, 0, 1) < prob_true;
}

void srsran_random_free(srsran_random_t q)
{
  if (q) {
    delete (random_wrap*)q;
  }
}
}
