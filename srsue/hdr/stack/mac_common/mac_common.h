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

#ifndef SRSUE_MAC_COMMON_H
#define SRSUE_MAC_COMMON_H

/**
 * @brief Common definitions/interfaces between LTE/NR MAC components
 *
 * @remark: So far only the trigger types are identical. The BSR report type and LCID mapping is implemented in RAT
 * specialications.
 */
namespace srsue {

// BSR trigger are common between LTE and NR
typedef enum { NONE, REGULAR, PADDING, PERIODIC } bsr_trigger_type_t;
char* bsr_trigger_type_tostring(bsr_trigger_type_t type);

} // namespace srsue

#endif // SRSUE_MAC_COMMON_H
