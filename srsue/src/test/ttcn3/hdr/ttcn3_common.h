// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

/*! \brief Common types for TTCN3 test systems
 *
 */

#ifndef SRSUE_TTCN3_COMMON_H
#define SRSUE_TTCN3_COMMON_H

#include <memory>

const static uint32_t                    RX_BUF_SIZE = 1024 * 1024;
typedef std::array<uint8_t, RX_BUF_SIZE> byte_array_t;
typedef std::unique_ptr<byte_array_t>    unique_byte_array_t;

#endif // SRSUE_TTCN3_COMMON_H
