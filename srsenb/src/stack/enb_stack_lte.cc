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

#include "srsenb/hdr/stack/enb_stack_lte.h"
#include "srsenb/hdr/common/rnti_pool.h"
#include "srsenb/hdr/enb.h"
#include "srsenb/hdr/stack/rrc/rrc_config_nr.h"
#include "srsran/interfaces/enb_metrics_interface.h"
#include "srsran/rlc/bearer_mem_pool.h"
#include "srsran/srslog/event_trace.h"

using namespace srsran;

namespace srsenb {

class gtpu_pdcp_adapter : public gtpu_interface_pdcp
{
public:
  gtpu_pdcp_adapter(gtpu* gtpu_, srsran::bearer_manager& bearers_) : gtpu_obj(gtpu_), bearers(&bearers_) {}

  /// Converts LCID to EPS-BearerID and sends corresponding PDU to GTPU
  void write_pdu(uint16_t rnti, uint32_t lcid, srsran::unique_byte_buffer_t pdu)
  {
    auto bearer = bearers->get_lcid_bearer(rnti, lcid);
    gtpu_obj->write_pdu(rnti, bearer.eps_bearer_id, std::move(pdu));
  }

private:
  gtpu*                   gtpu_obj = nullptr;
  srsran::bearer_manager* bearers  = nullptr;
};

enb_stack_lte::enb_stack_lte(srslog::sink& log_sink) :
  thread("STACK"),
  mac_logger(srslog::fetch_basic_logger("MAC", log_sink)),
  mac_nr_logger(srslog::fetch_basic_logger("MAC-NR", log_sink)),
  rlc_logger(srslog::fetch_basic_logger("RLC", log_sink, false)),
  rlc_nr_logger(srslog::fetch_basic_logger("RLC-NR", log_sink, false)),
  pdcp_logger(srslog::fetch_basic_logger("PDCP", log_sink, false)),
  pdcp_nr_logger(srslog::fetch_basic_logger("PDCP-NR", log_sink, false)),
  rrc_logger(srslog::fetch_basic_logger("RRC", log_sink, false)),
  rrc_nr_logger(srslog::fetch_basic_logger("RRC-NR", log_sink, false)),
  s1ap_logger(srslog::fetch_basic_logger("S1AP", log_sink, false)),
  gtpu_logger(srslog::fetch_basic_logger("GTPU", log_sink, false)),
  stack_logger(srslog::fetch_basic_logger("STCK", log_sink, false)),
  task_sched(512, 128),
  pdcp(&task_sched, pdcp_logger),
  pdcp_nr(&task_sched, pdcp_nr_logger),
  mac(&task_sched, mac_logger),
  mac_nr(&task_sched),
  rlc(rlc_logger),
  rlc_nr(rlc_nr_logger),
  gtpu(&task_sched, gtpu_logger, &rx_sockets),
  s1ap(&task_sched, s1ap_logger, &rx_sockets),
  rrc(this, &task_sched),
  rrc_nr(&task_sched),
  mac_pcap(),
  pending_stack_metrics(64)
{
  get_background_workers().set_nof_workers(2);
  enb_task_queue     = task_sched.make_task_queue();
  metrics_task_queue = task_sched.make_task_queue();
  // sync_queue is added in init()
}

enb_stack_lte::~enb_stack_lte()
{
  stop();
}

std::string enb_stack_lte::get_type()
{
  return "lte";
}

int enb_stack_lte::init(const stack_args_t&      args_,
                        const rrc_cfg_t&         rrc_cfg_,
                        phy_interface_stack_lte* phy_,
                        phy_interface_stack_nr*  phy_nr_)
{
  phy_nr = phy_nr_;
  if (init(args_, rrc_cfg_, phy_)) {
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

int enb_stack_lte::init(const stack_args_t& args_, const rrc_cfg_t& rrc_cfg_, phy_interface_stack_lte* phy_)
{
  phy = phy_;
  if (init(args_, rrc_cfg_)) {
    return SRSRAN_ERROR;
  }

  return SRSRAN_SUCCESS;
}

int enb_stack_lte::init(const stack_args_t& args_, const rrc_cfg_t& rrc_cfg_)
{
  args    = args_;
  rrc_cfg = rrc_cfg_;

  // Init RNTI and bearer memory pools
  reserve_rnti_memblocks(args.mac.nof_prealloc_ues);
  uint32_t min_nof_bearers_per_ue = 4;
  reserve_rlc_memblocks(args.mac.nof_prealloc_ues * min_nof_bearers_per_ue);

  // setup logging for each layer
  mac_logger.set_level(srslog::str_to_basic_level(args.log.mac_level));
  mac_nr_logger.set_level(srslog::str_to_basic_level(args.log.mac_level));
  rlc_logger.set_level(srslog::str_to_basic_level(args.log.rlc_level));
  rlc_nr_logger.set_level(srslog::str_to_basic_level(args.log.rlc_level));
  pdcp_logger.set_level(srslog::str_to_basic_level(args.log.pdcp_level));
  pdcp_nr_logger.set_level(srslog::str_to_basic_level(args.log.pdcp_level));
  rrc_logger.set_level(srslog::str_to_basic_level(args.log.rrc_level));
  rrc_nr_logger.set_level(srslog::str_to_basic_level(args.log.rrc_level));
  gtpu_logger.set_level(srslog::str_to_basic_level(args.log.gtpu_level));
  s1ap_logger.set_level(srslog::str_to_basic_level(args.log.s1ap_level));
  stack_logger.set_level(srslog::str_to_basic_level(args.log.stack_level));

  mac_logger.set_hex_dump_max_size(args.log.mac_hex_limit);
  mac_nr_logger.set_hex_dump_max_size(args.log.mac_hex_limit);
  rlc_logger.set_hex_dump_max_size(args.log.rlc_hex_limit);
  rlc_nr_logger.set_hex_dump_max_size(args.log.rlc_hex_limit);
  pdcp_logger.set_hex_dump_max_size(args.log.pdcp_hex_limit);
  pdcp_nr_logger.set_hex_dump_max_size(args.log.pdcp_hex_limit);
  rrc_logger.set_hex_dump_max_size(args.log.rrc_hex_limit);
  rrc_nr_logger.set_hex_dump_max_size(args.log.rrc_hex_limit);
  gtpu_logger.set_hex_dump_max_size(args.log.gtpu_hex_limit);
  s1ap_logger.set_hex_dump_max_size(args.log.s1ap_hex_limit);
  stack_logger.set_hex_dump_max_size(args.log.stack_hex_limit);

  // Set up pcap and trace
  if (args.mac_pcap.enable) {
    mac_pcap.open(args.mac_pcap.filename);
    mac.start_pcap(&mac_pcap);
  }

  if (args.mac_pcap_net.enable) {
    mac_pcap_net.open(args.mac_pcap_net.client_ip,
                      args.mac_pcap_net.bind_ip,
                      args.mac_pcap_net.client_port,
                      args.mac_pcap_net.bind_port);
    mac.start_pcap_net(&mac_pcap_net);
  }

  if (args.s1ap_pcap.enable) {
    s1ap_pcap.open(args.s1ap_pcap.filename.c_str());
    s1ap.start_pcap(&s1ap_pcap);
  }

  // add sync queue
  sync_task_queue = task_sched.make_task_queue(args.sync_queue_size);

  // setup bearer managers
  gtpu_adapter.reset(new gtpu_pdcp_adapter(&gtpu, bearers));

  // Init all LTE layers
  if (!mac.init(args.mac, rrc_cfg.cell_list, phy, &rlc, &rrc)) {
    stack_logger.error("Couldn't initialize MAC");
    return SRSRAN_ERROR;
  }
  rlc.init(&pdcp, &rrc, &mac, task_sched.get_timer_handler());
  pdcp.init(&rlc, &rrc, gtpu_adapter.get());
  if (rrc.init(rrc_cfg, phy, &mac, &rlc, &pdcp, &s1ap, &gtpu, &rrc_nr) != SRSRAN_SUCCESS) {
    stack_logger.error("Couldn't initialize RRC");
    return SRSRAN_ERROR;
  }
  if (s1ap.init(args.s1ap, &rrc) != SRSRAN_SUCCESS) {
    stack_logger.error("Couldn't initialize S1AP");
    return SRSRAN_ERROR;
  }

  // NR layers
  mac_nr_args_t mac_args = {};
  mac_args.fixed_dl_mcs  = 28;
  mac_args.fixed_ul_mcs  = 10;
  mac_args.pcap          = args.mac_pcap;
  mac_args.pcap.filename = "/tmp/enb_mac_nr.pcap";
  if (mac_nr.init(mac_args, phy_nr, nullptr, &rlc_nr, &rrc_nr) != SRSRAN_SUCCESS) {
    stack_logger.error("Couldn't initialize MAC-NR");
    return SRSRAN_ERROR;
  }

  rrc_nr_cfg_t rrc_cfg_nr = {};
  if (rrc_nr.init(rrc_cfg_nr, phy_nr, &mac_nr, &rlc_nr, &pdcp_nr, nullptr, nullptr, &rrc) != SRSRAN_SUCCESS) {
    stack_logger.error("Couldn't initialize RRC-NR");
    return SRSRAN_ERROR;
  }
  rlc_nr.init(&pdcp_nr, &rrc_nr, &mac_nr, task_sched.get_timer_handler());
  pdcp_nr.init(&rlc_nr, &rrc_nr, &gtpu);

  gtpu_args_t gtpu_args;
  gtpu_args.embms_enable                 = args.embms.enable;
  gtpu_args.embms_m1u_multiaddr          = args.embms.m1u_multiaddr;
  gtpu_args.embms_m1u_if_addr            = args.embms.m1u_if_addr;
  gtpu_args.mme_addr                     = args.s1ap.mme_addr;
  gtpu_args.gtp_bind_addr                = args.s1ap.gtp_bind_addr;
  gtpu_args.indirect_tunnel_timeout_msec = args.gtpu_indirect_tunnel_timeout_msec;
  if (gtpu.init(gtpu_args, this) != SRSRAN_SUCCESS) {
    stack_logger.error("Couldn't initialize GTPU");
    return SRSRAN_ERROR;
  }

  started = true;
  start(STACK_MAIN_THREAD_PRIO);

  return SRSRAN_SUCCESS;
}

void enb_stack_lte::tti_clock()
{
  sync_task_queue.push([this]() { tti_clock_impl(); });
}

void enb_stack_lte::tti_clock_impl()
{
  task_sched.tic();
  rrc.tti_clock();
}

void enb_stack_lte::stop()
{
  if (started) {
    enb_task_queue.push([this]() { stop_impl(); });
    wait_thread_finish();
  }
}

void enb_stack_lte::stop_impl()
{
  rx_sockets.stop();

  s1ap.stop();
  gtpu.stop();
  mac.stop();
  rlc.stop();
  pdcp.stop();
  rrc.stop();

  if (args.mac_pcap.enable) {
    mac_pcap.close();
  }

  if (args.mac_pcap_net.enable) {
    mac_pcap_net.close();
  }

  if (args.s1ap_pcap.enable) {
    s1ap_pcap.close();
  }

  task_sched.stop();
  get_background_workers().stop();

  started = false;
}

bool enb_stack_lte::get_metrics(stack_metrics_t* metrics)
{
  // use stack thread to query metrics
  auto ret = metrics_task_queue.try_push([this]() {
    stack_metrics_t metrics{};
    mac.get_metrics(metrics.mac);
    if (not metrics.mac.ues.empty()) {
      rlc.get_metrics(metrics.rlc, metrics.mac.ues[0].nof_tti);
      pdcp.get_metrics(metrics.pdcp, metrics.mac.ues[0].nof_tti);
    }
    rrc.get_metrics(metrics.rrc);
    s1ap.get_metrics(metrics.s1ap);
    if (not pending_stack_metrics.try_push(metrics)) {
      stack_logger.error("Unable to push metrics to queue");
    }
  });

  if (ret.has_value()) {
    // wait for result
    *metrics = pending_stack_metrics.pop_blocking();
    return true;
  }
  return false;
}

void enb_stack_lte::run_thread()
{
  while (started.load(std::memory_order_relaxed)) {
    task_sched.run_next_task();
  }
}

void enb_stack_lte::write_sdu(uint16_t rnti, uint32_t eps_bearer_id, srsran::unique_byte_buffer_t sdu, int pdcp_sn)
{
  auto bearer = bearers.get_radio_bearer(rnti, eps_bearer_id);
  // route SDU to PDCP entity
  if (bearer.rat == srsran_rat_t::lte) {
    pdcp.write_sdu(rnti, bearer.lcid, std::move(sdu), pdcp_sn);
  } else if (bearer.rat == srsran_rat_t::nr) {
    pdcp_nr.write_sdu(rnti, bearer.lcid, std::move(sdu), pdcp_sn);
  } else {
    stack_logger.warning("Can't deliver SDU for EPS bearer %d. Dropping it.", eps_bearer_id);
  }
}

std::map<uint32_t, srsran::unique_byte_buffer_t> enb_stack_lte::get_buffered_pdus(uint16_t rnti, uint32_t lcid)
{
  // TODO: use eps_bearer_id as lookup too
  return pdcp.get_buffered_pdus(rnti, lcid);
}

void enb_stack_lte::add_eps_bearer(uint16_t rnti, uint8_t eps_bearer_id, srsran::srsran_rat_t rat, uint32_t lcid)
{
  bearers.add_eps_bearer(rnti, eps_bearer_id, rat, lcid);
}

void enb_stack_lte::remove_eps_bearer(uint16_t rnti, uint8_t eps_bearer_id)
{
  bearers.remove_eps_bearer(rnti, eps_bearer_id);
}

void enb_stack_lte::reset_eps_bearers(uint16_t rnti)
{
  bearers.reset(rnti);
}

} // namespace srsenb
