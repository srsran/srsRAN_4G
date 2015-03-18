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


#ifndef _LTE_
#define _LTE_

#ifdef __cplusplus
    extern "C" {
#endif

#include <complex.h>
#include <math.h>

#include "srslte/config.h"

#include "srslte/phy/utils/bit.h"
#include "srslte/phy/utils/convolution.h"
#include "srslte/phy/utils/debug.h"
#include "srslte/phy/utils/dft.h"
#include "srslte/phy/utils/matrix.h"
#include "srslte/phy/utils/mux.h"
#include "srslte/phy/utils/cexptab.h"
#include "srslte/phy/utils/pack.h"
#include "srslte/phy/utils/vector.h"

#include "srslte/phy/common/timestamp.h"
#include "srslte/phy/common/sequence.h"
#include "srslte/phy/common/phy_common.h"
#include "srslte/phy/common/fft.h"
            
#include "srslte/phy/ch_estimation/chest_dl.h"
#include "srslte/phy/ch_estimation/refsignal_dl.h"
#include "srslte/phy/ch_estimation/refsignal_ul.h"

#include "srslte/phy/resampling/interp.h"
#include "srslte/phy/resampling/decim.h"
#include "srslte/phy/resampling/resample_arb.h"

#include "srslte/phy/channel/ch_awgn.h"

#include "srslte/phy/fec/viterbi.h"
#include "srslte/phy/fec/convcoder.h"
#include "srslte/phy/fec/crc.h"
#include "srslte/phy/fec/tc_interl.h"
#include "srslte/phy/fec/turbocoder.h"
#include "srslte/phy/fec/turbodecoder.h"
#include "srslte/phy/fec/rm_conv.h"
#include "srslte/phy/fec/rm_turbo.h"

#include "srslte/phy/filter/filter2d.h"
#include "srslte/phy/filter/dft_precoding.h"

#include "srslte/phy/io/binsource.h"
#include "srslte/phy/io/filesink.h"
#include "srslte/phy/io/filesource.h"
#include "srslte/phy/io/netsink.h"
#include "srslte/phy/io/netsource.h"

#include "srslte/phy/modem/demod_hard.h"
#include "srslte/phy/modem/demod_soft.h"
#include "srslte/phy/modem/mod.h"
#include "srslte/phy/modem/modem_table.h"

#include "srslte/phy/mimo/precoding.h"
#include "srslte/phy/mimo/layermap.h"

#include "srslte/phy/phch/cqi.h"
#include "srslte/phy/phch/dci.h"
#include "srslte/phy/phch/harq.h"
#include "srslte/phy/phch/pbch.h"
#include "srslte/phy/phch/pcfich.h"
#include "srslte/phy/phch/pdcch.h"
#include "srslte/phy/phch/pdsch.h"
#include "srslte/phy/phch/phich.h"
#include "srslte/phy/phch/pusch.h"
#include "srslte/phy/phch/prach.h"
#include "srslte/phy/phch/ra.h"
#include "srslte/phy/phch/regs.h"
#include "srslte/phy/phch/sch.h"
#include "srslte/phy/phch/uci.h"
      
#include "srslte/phy/ue/ue_sync.h"
#include "srslte/phy/ue/ue_mib.h"
#include "srslte/phy/ue/ue_cell_search.h"
#include "srslte/phy/ue/ue_dl.h"
#include "srslte/phy/ue/ue_ul.h"

#include "srslte/phy/scrambling/scrambling.h"

#include "srslte/phy/sync/pss.h"
#include "srslte/phy/sync/sfo.h"
#include "srslte/phy/sync/sss.h"
#include "srslte/phy/sync/sync.h"
#include "srslte/phy/sync/cfo.h"

#ifdef __cplusplus
}
#undef I // Fix complex.h #define I nastiness when using C++
#endif

#endif
