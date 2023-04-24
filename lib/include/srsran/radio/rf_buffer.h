/**
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#ifndef SRSRAN_RF_BUFFER_H
#define SRSRAN_RF_BUFFER_H

#include "srsran/interfaces/radio_interfaces.h"

namespace srsran {

/**
 * Implemenation of the rf_buffer_interface for the current radio implementation which uses flat arrays.
 * @see rf_buffer_interface
 * @see radio
 *
 */
class rf_buffer_t : public rf_buffer_interface
{
public:
  /**
   * Creates an object and allocates memory for nof_subframes_ assuming the
   * largest system bandwidth
   * @param nof_subframes_ Number of subframes to allocate
   */
  explicit rf_buffer_t(uint32_t nof_subframes_)
  {
    if (nof_subframes_ > 0) {
      // Allocate buffers for an integer number of subframes
      for (uint32_t i = 0; i < SRSRAN_MAX_CHANNELS; i++) {
        sample_buffer[i] = srsran_vec_cf_malloc(nof_subframes_ * SRSRAN_SF_LEN_MAX);
        srsran_vec_cf_zero(sample_buffer[i], SRSRAN_SF_LEN_MAX);
      }
      allocated     = true;
      nof_subframes = nof_subframes_;
    }
  }
  /**
   * Creates an object and sets the buffers to the flat array pointed by data. Note that data must
   * contain up to SRSRAN_MAX_CHANNELS pointers
   * @param data Flat array to use as initializer for the internal buffer pointers
   */
  explicit rf_buffer_t(cf_t* data[SRSRAN_MAX_CHANNELS], uint32_t nof_samples_)
  {
    for (uint32_t i = 0; i < SRSRAN_MAX_CHANNELS; i++) {
      sample_buffer[i] = data[i];
    }
    nof_samples = nof_samples_;
  }
  /**
   * Creates an object from a single array pointer. The rest of the channel pointers will be left to NULL
   * @param data Flat array to use as initializer for the internal buffer pointers
   */
  explicit rf_buffer_t(cf_t* data, uint32_t nof_samples_)
  {
    sample_buffer[0] = data;
    nof_samples      = nof_samples_;
  }
  /**
   * Default constructor leaves the internal pointers to NULL
   */
  rf_buffer_t() = default;

  /**
   * The destructor will deallocate memory only if it was allocated passing nof_subframes > 0
   */
  ~rf_buffer_t()
  {
    if (allocated) {
      free_all();
    }
  }
  /**
   * Overrides the = operator such that the lvalue internal buffers point to the pointers inside rvalue.
   * If memory has already been allocated in the lvalue object, it will free it before pointing the
   * buffers to the lvalue.
   * After this operator, when the lvalue is destroyed no memory will be freed.
   * @param other rvalue
   * @return lvalue
   */
  rf_buffer_t& operator=(const rf_buffer_t& other)
  {
    if (this == &other) {
      return *this;
    }
    if (this->allocated) {
      free_all();
      this->allocated = false;
    }
    for (int i = 0; i < SRSRAN_MAX_CHANNELS; i++) {
      this->sample_buffer[i] = other.sample_buffer[i];
    }
    return *this;
  }

  rf_buffer_t(const rf_buffer_t& other) = delete;
  cf_t* get(const uint32_t& channel_idx) const override { return sample_buffer.at(channel_idx); }
  void  set(const uint32_t& channel_idx, cf_t* ptr) override { sample_buffer.at(channel_idx) = ptr; }
  cf_t* get(const uint32_t& logical_ch, const uint32_t& port_idx, const uint32_t& nof_antennas) const override
  {
    return sample_buffer.at(logical_ch * nof_antennas + port_idx);
  }
  void set(const uint32_t& logical_ch, const uint32_t& port_idx, const uint32_t& nof_antennas, cf_t* ptr) override
  {
    sample_buffer.at(logical_ch * nof_antennas + port_idx) = ptr;
  }
  void set_combine(const uint32_t& channel_idx, cf_t* ptr)
  {
    if (sample_buffer.at(channel_idx) == nullptr) {
      sample_buffer.at(channel_idx) = ptr;
    } else if (ptr != nullptr) {
      srsran_vec_sum_ccc(ptr, sample_buffer.at(channel_idx), sample_buffer.at(channel_idx), nof_samples);
    }
  }
  void set_combine(const uint32_t& logical_ch, const uint32_t& port_idx, const uint32_t& nof_antennas, cf_t* ptr)
  {
    set_combine(logical_ch * nof_antennas + port_idx, ptr);
  }
  void set_combine(const rf_buffer_interface& other)
  {
    // Take the other number of samples always
    set_nof_samples(other.get_nof_samples());
    for (uint32_t ch = 0; ch < SRSRAN_MAX_CHANNELS; ch++) {
      set_combine(ch, other.get(ch));
    }
  }
  void**   to_void() override { return (void**)sample_buffer.data(); }
  cf_t**   to_cf_t() override { return sample_buffer.data(); }
  uint32_t size() override { return nof_subframes * SRSRAN_SF_LEN_MAX; }
  void     set_nof_samples(uint32_t n) override { nof_samples = n; }
  uint32_t get_nof_samples() const override { return nof_samples; }

private:
  std::array<cf_t*, SRSRAN_MAX_CHANNELS> sample_buffer = {};
  bool                                   allocated     = false;
  uint32_t                               nof_subframes = 0;
  uint32_t                               nof_samples   = 0;
  void                                   free_all()
  {
    for (uint32_t i = 0; i < SRSRAN_MAX_CHANNELS; i++) {
      if (sample_buffer[i]) {
        free(sample_buffer[i]);
      }
    }
  }
};

} // namespace srsran

#endif // SRSRAN_RF_BUFFER_H
