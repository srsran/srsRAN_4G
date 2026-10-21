// SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
// SPDX-License-Identifier: LicenseRef-SRS-Custom

#ifndef SRSRAN_SLIV_H
#define SRSRAN_SLIV_H

#include "srsran/config.h"
#include <inttypes.h>

SRSRAN_API void srsran_sliv_to_s_and_l(uint32_t N, uint32_t v, uint32_t* S, uint32_t* L);

SRSRAN_API uint32_t srsran_sliv_from_s_and_l(uint32_t N, uint32_t S, uint32_t L);

#endif // SRSRAN_SLIV_H
