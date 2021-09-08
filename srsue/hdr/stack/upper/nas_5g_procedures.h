/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSUE_NAS_5G_PROCEDURES_H_
#define SRSUE_NAS_5G_PROCEDURES_H_

#include "srsue/hdr/stack/upper/nas_5g.h"

namespace srsue {

/**
 * @brief 5G NAS registration procedure
 *
 * Specified in 24 501 V16.7.0
 * 5GMM specific procedures
 * 5.5.1 Registration procedure
 */
class nas_5g::registration_procedure
{
public:
  explicit registration_procedure(nas_5g_interface_procedures* parent_nas_);
  srsran::proc_outcome_t init();
  srsran::proc_outcome_t step();
  srsran::proc_outcome_t then();
  static const char*     name() { return "Registration Procedure"; }

private:
  nas_5g_interface_procedures* parent_nas;
};
} // namespace srsue

#endif // SRSUE_NAS_5G_PROCEDURES_H_