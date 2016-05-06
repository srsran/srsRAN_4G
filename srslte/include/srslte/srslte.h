/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsLTE library.
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


#ifndef _LTE_
#define _LTE_

#ifdef __cplusplus
    extern "C" {
#endif
      
#include <complex.h>
#include <math.h>

#include "srslte/config.h"
#include "srslte/version.h"

#include "srslte/utils/bit.h"
#include "srslte/utils/convolution.h"
#include "srslte/utils/debug.h"
#include "srslte/utils/cexptab.h"
#include "srslte/utils/vector.h"

#include "srslte/common/timestamp.h"
#include "srslte/common/sequence.h"
#include "srslte/common/phy_common.h"
            
#include "srslte/ch_estimation/chest_ul.h"
#include "srslte/ch_estimation/chest_dl.h"
#include "srslte/ch_estimation/refsignal_dl.h"
#include "srslte/ch_estimation/refsignal_ul.h"

#include "srslte/resampling/interp.h"
#include "srslte/resampling/decim.h"
#include "srslte/resampling/resample_arb.h"

#include "srslte/channel/ch_awgn.h"

#include "srslte/fec/viterbi.h"
#include "srslte/fec/convcoder.h"
#include "srslte/fec/crc.h"
#include "srslte/fec/tc_interl.h"
#include "srslte/fec/turbocoder.h"
#include "srslte/fec/turbodecoder.h"
#include "srslte/fec/cbsegm.h"
#include "srslte/fec/rm_conv.h"
#include "srslte/fec/rm_turbo.h"

#include "srslte/dft/dft_precoding.h"
#include "srslte/dft/ofdm.h"
#include "srslte/dft/dft.h"

#include "srslte/io/binsource.h"
#include "srslte/io/filesink.h"
#include "srslte/io/filesource.h"
#include "srslte/io/netsink.h"
#include "srslte/io/netsource.h"

#include "srslte/modem/demod_hard.h"
#include "srslte/modem/demod_soft.h"
#include "srslte/modem/mod.h"
#include "srslte/modem/modem_table.h"

#include "srslte/mimo/precoding.h"
#include "srslte/mimo/layermap.h"

#include "srslte/phch/cqi.h"
#include "srslte/phch/dci.h"
#include "srslte/fec/softbuffer.h"
#include "srslte/phch/pbch.h"
#include "srslte/phch/pcfich.h"
#include "srslte/phch/pdcch.h"
#include "srslte/phch/pdsch.h"
#include "srslte/phch/phich.h"
#include "srslte/phch/pusch.h"
#include "srslte/phch/pucch.h"
#include "srslte/phch/prach.h"
#include "srslte/phch/ra.h"
#include "srslte/phch/regs.h"
#include "srslte/phch/sch.h"
#include "srslte/phch/uci.h"
      
#include "srslte/ue/ue_sync.h"
#include "srslte/ue/ue_mib.h"
#include "srslte/ue/ue_cell_search.h"
#include "srslte/ue/ue_dl.h"
#include "srslte/ue/ue_ul.h"

#include "srslte/enb/enb_dl.h"
#include "srslte/enb/enb_ul.h"

#include "srslte/scrambling/scrambling.h"

#include "srslte/sync/pss.h"
#include "srslte/sync/sfo.h"
#include "srslte/sync/sss.h"
#include "srslte/sync/sync.h"
#include "srslte/sync/cfo.h"
#include "srslte/sync/cp.h"

#ifdef __cplusplus
}
#undef I // Fix complex.h #define I nastiness when using C++
#endif

#endif
