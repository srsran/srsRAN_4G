/**
 * Copyright 2013-2021 Software Radio Systems Limited
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

#include "srsue/hdr/stack/ue_stack_nr.h"
#include "srsran/srsran.h"
#include "srsue/hdr/stack/rrc/rrc_nr.h"

using namespace srsran;

namespace srsue {

ue_stack_nr::ue_stack_nr() :
  thread("STACK"),
  task_sched(64, 64),
  mac_logger(srslog::fetch_basic_logger("MAC-NR")),
  rlc_logger(srslog::fetch_basic_logger("RLC-NR", false)),
  pdcp_logger(srslog::fetch_basic_logger("PDCP-NR", false))
{
  get_background_workers().set_nof_workers(2);
  mac.reset(new mac_nr(&task_sched));
  pdcp.reset(new srsran::pdcp(&task_sched, "PDCP-NR"));
  rlc.reset(new srsran::rlc("RLC-NR"));
  rrc.reset(new rrc_nr(&task_sched));

  // setup logging for pool, RLC and PDCP
  byte_buffer_pool::get_instance()->enable_logger(true);

  ue_task_queue   = task_sched.make_task_queue();
  sync_task_queue = task_sched.make_task_queue();
  gw_task_queue   = task_sched.make_task_queue();
}

ue_stack_nr::~ue_stack_nr()
{
  stop();
}

std::string ue_stack_nr::get_type()
{
  return "nr";
}

int ue_stack_nr::init(const stack_args_t& args_, phy_interface_stack_nr* phy_, gw_interface_stack* gw_)
{
  phy = phy_;
  gw  = gw_;
  return init(args_);
}

int ue_stack_nr::init(const stack_args_t& args_)
{
  args = args_;

  mac_logger.set_level(srslog::str_to_basic_level(args.log.mac_level));
  mac_logger.set_hex_dump_max_size(args.log.mac_hex_limit);
  rlc_logger.set_level(srslog::str_to_basic_level(args.log.rlc_level));
  rlc_logger.set_hex_dump_max_size(args.log.rlc_hex_limit);
  pdcp_logger.set_level(srslog::str_to_basic_level(args.log.pdcp_level));
  pdcp_logger.set_hex_dump_max_size(args.log.pdcp_hex_limit);

  mac_nr_args_t mac_args = {};
  mac->init(mac_args, phy, rlc.get(), rrc.get());
  rlc->init(pdcp.get(), rrc.get(), task_sched.get_timer_handler(), 0 /* RB_ID_SRB0 */);
  pdcp->init(rlc.get(), rrc.get(), gw);

  // TODO: where to put RRC args?
  rrc_nr_args_t rrc_args     = {};
  rrc_args.log_level         = args.log.rrc_level;
  rrc_args.log_hex_limit     = args.log.rrc_hex_limit;
  rrc_args.coreless.drb_lcid = 4;
  rrc_args.coreless.ip_addr  = "192.168.1.3";
  rrc->init(
      phy, mac.get(), rlc.get(), pdcp.get(), gw, nullptr, nullptr, task_sched.get_timer_handler(), this, rrc_args);
  rrc->init_core_less();
  running = true;
  start(STACK_MAIN_THREAD_PRIO);

  return SRSRAN_SUCCESS;
}

void ue_stack_nr::stop()
{
  if (running) {
    ue_task_queue.try_push([this]() { stop_impl(); });
    wait_thread_finish();
  }
}

void ue_stack_nr::stop_impl()
{
  running = false;

  rrc->stop();

  rlc->stop();
  pdcp->stop();
  mac->stop();

  get_background_workers().stop();
}

bool ue_stack_nr::switch_on()
{
  // statically setup TUN (will be done through RRC later)
  char*          err_str = nullptr;
  struct in_addr in_addr;
  if (inet_pton(AF_INET, "192.168.1.3", &in_addr.s_addr) != 1) {
    perror("inet_pton");
    return false;
  }
  if (gw->setup_if_addr(5, LIBLTE_MME_PDN_TYPE_IPV4, htonl(in_addr.s_addr), nullptr, err_str)) {
    printf("Error configuring TUN interface\n");
  }
  return true;
}

bool ue_stack_nr::switch_off()
{
  return true;
}

bool ue_stack_nr::get_metrics(stack_metrics_t* metrics)
{
  // mac.get_metrics(metrics->mac);
  rlc->get_metrics(metrics->rlc, metrics->mac[0].nof_tti);
  // rrc.get_metrics(metrics->rrc);
  return true;
}

void ue_stack_nr::run_thread()
{
  while (running) {
    task_sched.run_next_task();
  }
}

/***********************************************************************************************************************
 *                                                Stack Interfaces
 **********************************************************************************************************************/

/********************
 *   GW Interface
 *******************/

/**
 * Push GW SDU to stack
 * @param lcid
 * @param sdu
 * @param blocking
 */
void ue_stack_nr::write_sdu(uint32_t lcid, srsran::unique_byte_buffer_t sdu)
{
  if (pdcp != nullptr) {
    auto ret = gw_task_queue.try_push(std::bind(
        [this, lcid](srsran::unique_byte_buffer_t& sdu) { pdcp->write_sdu(lcid, std::move(sdu)); }, std::move(sdu)));
    if (ret.is_error()) {
      pdcp_logger.warning("GW SDU with lcid=%d was discarded.", lcid);
    }
  }
}

/********************
 *  SYNC Interface
 *******************/

/**
 * Sync thread signal that it is in sync
 */
void ue_stack_nr::in_sync()
{
  // pending_tasks.push(sync_task_queue, task_t{[this](task_t*) { rrc.in_sync(); }});
}

void ue_stack_nr::out_of_sync()
{
  // pending_tasks.push(sync_task_queue, task_t{[this](task_t*) { rrc.out_of_sync(); }});
}

void ue_stack_nr::run_tti(uint32_t tti)
{
  sync_task_queue.push([this, tti]() { run_tti_impl(tti); });
}

void ue_stack_nr::run_tti_impl(uint32_t tti)
{
  mac->run_tti(tti);
  rrc->run_tti(tti);
  task_sched.tic();
}

void ue_stack_nr::set_phy_config_complete(bool status)
{
  sync_task_queue.push([this, status]() { rrc->set_phy_config_complete(status); });
}

} // namespace srsue
