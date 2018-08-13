/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#include "srsue/hdr/ue_base.h"
#include "srsue/hdr/ue.h"
#include "srslte/srslte.h"
#include "srslte/build_info.h"
#include <pthread.h>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>

using namespace srslte;

namespace srsue{

static ue_base* instance = NULL;
pthread_mutex_t ue_instance_mutex = PTHREAD_MUTEX_INITIALIZER;

ue_base* ue_base::get_instance(srsue_instance_type_t type)
{
  pthread_mutex_lock(&ue_instance_mutex);
  if(NULL == instance) {
    switch (type) {
      case LTE:
        instance = new ue();
        break;
      default:
        perror("Unknown UE type.\n");
    }
  }
  pthread_mutex_unlock(&ue_instance_mutex);
  return(instance);
}

ue_base::ue_base() {
  // print build info
  std::cout << std::endl << get_build_string() << std::endl;

  // load FFTW wisdom
  srslte_dft_load();

  pool = byte_buffer_pool::get_instance();
}

ue_base::~ue_base() {
  byte_buffer_pool::cleanup();
}

void ue_base::cleanup(void)
{
  // save FFTW wisdom
  srslte_dft_exit();

  pthread_mutex_lock(&ue_instance_mutex);
  if(NULL != instance) {
    delete instance;
    instance = NULL;
  }
  pthread_mutex_unlock(&ue_instance_mutex);
}

void ue_base::handle_rf_msg(srslte_rf_error_t error)
{
  if(error.type == srslte_rf_error_t::SRSLTE_RF_ERROR_OVERFLOW) {
    rf_metrics.rf_o++;
    rf_metrics.rf_error = true;
    rf_log.warning("Overflow\n");
  }else if(error.type == srslte_rf_error_t::SRSLTE_RF_ERROR_UNDERFLOW) {
    rf_metrics.rf_u++;
    rf_metrics.rf_error = true;
    rf_log.warning("Underflow\n");
  } else if(error.type == srslte_rf_error_t::SRSLTE_RF_ERROR_LATE) {
    rf_metrics.rf_l++;
    rf_metrics.rf_error = true;
    rf_log.warning("Late (detected in %s)\n", error.opt?"rx call":"asynchronous thread");
  } else if (error.type == srslte_rf_error_t::SRSLTE_RF_ERROR_OTHER) {
    std::string str(error.msg);
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
    str.push_back('\n');
    rf_log.info("%s\n", str.c_str());
  }
}

srslte::LOG_LEVEL_ENUM ue_base::level(std::string l)
{
  std::transform(l.begin(), l.end(), l.begin(), ::toupper);
  if("NONE" == l){
    return srslte::LOG_LEVEL_NONE;
  }else if("ERROR" == l){
    return srslte::LOG_LEVEL_ERROR;
  }else if("WARNING" == l){
    return srslte::LOG_LEVEL_WARNING;
  }else if("INFO" == l){
    return srslte::LOG_LEVEL_INFO;
  }else if("DEBUG" == l){
    return srslte::LOG_LEVEL_DEBUG;
  }else{
    return srslte::LOG_LEVEL_NONE;
  }
}

std::string ue_base::get_build_mode()
{
  return std::string(srslte_get_build_mode());
}

std::string ue_base::get_build_info()
{
  if (std::string(srslte_get_build_info()) == "") {
    return std::string(srslte_get_version());
  }
  return std::string(srslte_get_build_info());
}

std::string ue_base::get_build_string()
{
  std::stringstream ss;
  ss << "Built in " << get_build_mode() << " mode using " << get_build_info() << "." << std::endl;
  return ss.str();
}

} // namespace srsue
