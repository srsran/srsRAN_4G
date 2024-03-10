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

#ifndef SRSRAN_PACKED_SIB_BUFFER_H
#define SRSRAN_PACKED_SIB_BUFFER_H

#include "srsran/common/byte_buffer.h"
#include "srsran/support/srsran_assert.h"

namespace srsenb {

/// \brief Buffer that holds SIBs in packed form.
///
/// Different versions of the same SIB can be stored in the buffer in the form of segments. This is useful for SIBs
/// which have its content split across different segments that are broadcast in sequence, such as SIBs 11 and 12. At
/// construction time, the packed SIB consists of a single segment.
class packed_sib_buffer
{
public:
  packed_sib_buffer() = delete;

  /// Constructor that takes ownership a buffer holding the SIB.
  explicit packed_sib_buffer(srsran::unique_byte_buffer_t&& buffer) { buffers.emplace_back(std::move(buffer)); }

  /// \brief Gets the contents of the SIB.
  /// \return A reference to the SIB buffer.
  /// \remark An assertion is thrown if the SIB has more than one segment.
  const srsran::unique_byte_buffer_t& get() const
  {
    srsran_assert(!is_segmented(), "SIB message is segmented.");
    return buffers[0];
  }

  /// \brief Gets the contents of an SIB segment.
  /// \param[in] i_segment Segment index.
  /// \return A reference to the SIB segment buffer.
  /// \remark An assertion is thrown if the SIB is not segmented, or if the segment index exceeds the available
  /// segments.
  const srsran::unique_byte_buffer_t& get_segment(unsigned i_segment) const
  {
    srsran_assert(is_segmented(), "SIB message is not segmented.");
    srsran_assert(i_segment < buffers.size(), "SIB segment reading index overflow.");
    return buffers[i_segment];
  }

  /// \brief Sets a new SIB segment.
  /// \param[in] segment A buffer holding the SIB segment. Ownership is transferred to the \c packed_sib_buffer.
  void set_segment(srsran::unique_byte_buffer_t segment) { buffers.emplace_back(std::move(segment)); }

  /// Returns \c true if the SIB has more than one segment, \c false otherwise.
  bool is_segmented() const { return buffers.size() > 1; }

  /// Returns the number of currently stored SIB segments.
  unsigned get_nof_segments() const { return buffers.size(); }

private:
  /// SIB buffers. Each buffer stores an SIB segment.
  std::vector<srsran::unique_byte_buffer_t> buffers;
};

} // namespace srsenb

#endif // SRSRAN_PACKED_SIB_BUFFER_H
