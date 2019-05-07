/*
 * Copyright 2013-2019 Software Radio Systems Limited
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

usim_base* usim_base::get_instance(usim_args_t* args)
{
  usim_base* instance = NULL;
  if (args->mode == "soft") {
    instance = new usim();
  }
#if HAVE_PCSC
  else if (args->mode == "pcsc") {
    instance = new pcsc_usim();
  }
#endif
  else {
    // default to soft USIM
    instance = new usim();
  }
  return(instance);
}

usim_base::usim_base() {
}

usim_base::~usim_base() {
}

} // namespace srsue
