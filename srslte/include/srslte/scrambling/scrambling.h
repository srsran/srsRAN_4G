/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2014 The srsLTE Developers. See the
 * COPYRIGHT file at the top-level directory of this distribution.
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
 *
 * srsLTE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsLTE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * A copy of the GNU Lesser General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */


#ifndef SCRAMBLING_
#define SCRAMBLING_

#include "srslte/config.h"
#include "srslte/common/sequence.h"
#include "srslte/common/phy_common.h"

/* Scrambling has no state */
SRSLTE_API void srslte_scrambling_b(srslte_sequence_t *s, 
                                    uint8_t *data);

SRSLTE_API void srslte_scrambling_b_offset(srslte_sequence_t *s, 
                                           uint8_t *data, 
                                           int offset, 
                                           int len);

SRSLTE_API void srslte_scrambling_b_offset_pusch(srslte_sequence_t *s, 
                                                 uint8_t *data, 
                                                 int offset, 
                                                 int len); 

SRSLTE_API void srslte_scrambling_f(srslte_sequence_t *s, 
                                    float *data);

SRSLTE_API void srslte_scrambling_f_offset(srslte_sequence_t *s, 
                                           float *data, 
                                           int offset, 
                                           int len);

SRSLTE_API void srslte_scrambling_c(srslte_sequence_t *s, 
                                    cf_t *data);

SRSLTE_API void srslte_scrambling_c_offset(srslte_sequence_t *s, 
                                           cf_t *data, 
                                           int offset, 
                                           int len);


#endif // SCRAMBLING_
