/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

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
