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

#ifndef SRSRAN_SRSRAN_H
#define SRSRAN_SRSRAN_H

#ifdef __cplusplus
#include <complex>
extern "C" {
#else
#include <complex.h>
#endif

#include <math.h>

#include "srsran/config.h"
#include "srsran/version.h"

#include "srsran/phy/utils/bit.h"
#include "srsran/phy/utils/cexptab.h"
#include "srsran/phy/utils/convolution.h"
#include "srsran/phy/utils/debug.h"
#include "srsran/phy/utils/ringbuffer.h"
#include "srsran/phy/utils/vector.h"

#include "srsran/phy/common/phy_common.h"
#include "srsran/phy/common/sequence.h"
#include "srsran/phy/common/timestamp.h"
#include "srsran/phy/utils/phy_logger.h"

#include "srsran/phy/ch_estimation/cedron_freq_estimator.h"
#include "srsran/phy/ch_estimation/chest_dl.h"
#include "srsran/phy/ch_estimation/chest_ul.h"
#include "srsran/phy/ch_estimation/csi_rs.h"
#include "srsran/phy/ch_estimation/dmrs_pdcch.h"
#include "srsran/phy/ch_estimation/dmrs_sch.h"
#include "srsran/phy/ch_estimation/refsignal_dl.h"
#include "srsran/phy/ch_estimation/refsignal_ul.h"
#include "srsran/phy/ch_estimation/wiener_dl.h"

#include "srsran/phy/resampling/decim.h"
#include "srsran/phy/resampling/interp.h"
#include "srsran/phy/resampling/resample_arb.h"

#include "srsran/phy/channel/ch_awgn.h"

#include "srsran/phy/cfr/cfr.h"
#include "srsran/phy/dft/dft.h"
#include "srsran/phy/dft/dft_precoding.h"
#include "srsran/phy/dft/ofdm.h"
#include "srsran/phy/fec/cbsegm.h"
#include "srsran/phy/fec/convolutional/convcoder.h"
#include "srsran/phy/fec/convolutional/rm_conv.h"
#include "srsran/phy/fec/convolutional/viterbi.h"
#include "srsran/phy/fec/crc.h"
#include "srsran/phy/fec/turbo/rm_turbo.h"
#include "srsran/phy/fec/turbo/tc_interl.h"
#include "srsran/phy/fec/turbo/turbocoder.h"
#include "srsran/phy/fec/turbo/turbodecoder.h"

#include "srsran/phy/io/binsource.h"
#include "srsran/phy/io/filesink.h"
#include "srsran/phy/io/filesource.h"
#include "srsran/phy/io/netsink.h"
#include "srsran/phy/io/netsource.h"

#include "srsran/phy/modem/demod_hard.h"
#include "srsran/phy/modem/demod_soft.h"
#include "srsran/phy/modem/evm.h"
#include "srsran/phy/modem/mod.h"
#include "srsran/phy/modem/modem_table.h"

#include "srsran/phy/mimo/layermap.h"
#include "srsran/phy/mimo/precoding.h"

#include "srsran/phy/fec/softbuffer.h"
#include "srsran/phy/phch/cqi.h"
#include "srsran/phy/phch/csi.h"
#include "srsran/phy/phch/dci.h"
#include "srsran/phy/phch/dci_nr.h"
#include "srsran/phy/phch/harq_ack.h"
#include "srsran/phy/phch/pbch.h"
#include "srsran/phy/phch/pbch_nr.h"
#include "srsran/phy/phch/pcfich.h"
#include "srsran/phy/phch/pdcch.h"
#include "srsran/phy/phch/pdcch_nr.h"
#include "srsran/phy/phch/pdsch.h"
#include "srsran/phy/phch/phich.h"
#include "srsran/phy/phch/prach.h"
#include "srsran/phy/phch/pucch.h"
#include "srsran/phy/phch/pucch_proc.h"
#include "srsran/phy/phch/pusch.h"
#include "srsran/phy/phch/ra.h"
#include "srsran/phy/phch/ra_dl.h"
#include "srsran/phy/phch/ra_dl_nr.h"
#include "srsran/phy/phch/ra_nr.h"
#include "srsran/phy/phch/ra_ul.h"
#include "srsran/phy/phch/ra_ul_nr.h"
#include "srsran/phy/phch/regs.h"
#include "srsran/phy/phch/sch.h"
#include "srsran/phy/phch/uci.h"
#include "srsran/phy/phch/uci_nr.h"

#include "srsran/phy/ue/ue_cell_search.h"
#include "srsran/phy/ue/ue_dl.h"
#include "srsran/phy/ue/ue_dl_nr.h"
#include "srsran/phy/ue/ue_mib.h"
#include "srsran/phy/ue/ue_sync.h"
#include "srsran/phy/ue/ue_sync_nr.h"
#include "srsran/phy/ue/ue_ul.h"
#include "srsran/phy/ue/ue_ul_nr.h"

#include "srsran/phy/enb/enb_dl.h"
#include "srsran/phy/enb/enb_ul.h"
#include "srsran/phy/gnb/gnb_dl.h"
#include "srsran/phy/gnb/gnb_ul.h"

#include "srsran/phy/scrambling/scrambling.h"

#include "srsran/phy/sync/cfo.h"
#include "srsran/phy/sync/cp.h"
#include "srsran/phy/sync/pss.h"
#include "srsran/phy/sync/refsignal_dl_sync.h"
#include "srsran/phy/sync/sfo.h"
#include "srsran/phy/sync/ssb.h"
#include "srsran/phy/sync/sss.h"
#include "srsran/phy/sync/sync.h"

#ifdef __cplusplus
}
#undef I // Fix complex.h #define I nastiness when using C++
#endif

#endif // SRSRAN_SRSRAN_H
