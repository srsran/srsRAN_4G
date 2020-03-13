/*
 * Copyright 2013-2020 Software Radio Systems Limited
 *
 * This file is part of srsLTE.
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

#include "srsue/hdr/stack/upper/usim_base.h"
#include "srsue/hdr/stack/upper/usim.h"

#ifdef HAVE_PCSC
#include "srsue/hdr/stack/upper/pcsc_usim.h"
#endif

namespace srsue {

std::unique_ptr<usim_base> usim_base::get_instance(usim_args_t* args, srslte::log* log_)
{
#if HAVE_PCSC
  if (args->mode == "pcsc") {
    return std::unique_ptr<usim_base>(new pcsc_usim(log_));
  }
#endif

  // default to soft USIM
  return std::unique_ptr<usim_base>(new usim(log_));
}

usim_base::usim_base() {}

usim_base::~usim_base() {}

} // namespace srsue
