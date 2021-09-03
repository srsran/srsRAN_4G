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

#include "srsran/common/enb_events.h"
#include "srsran/srslog/context.h"
#include "srsran/srslog/log_channel.h"
#include <mutex>
#include <unordered_map>

using namespace srsenb;

namespace {

/// Null object implementation that is used when no log channel is configured.
class null_event_logger : public event_logger_interface
{
public:
  void log_rrc_event(uint32_t           enb_cc_idx,
                     const std::string& asn1_oct_str,
                     const std::string& asn1_txt_str,
                     unsigned           type,
                     unsigned           additional_info,
                     uint16_t           rnti) override
  {}
  void log_s1_ctx_create(uint32_t enb_cc_idx, uint32_t mme_id, uint32_t enb_id, uint16_t rnti) override {}
  void log_s1_ctx_delete(uint32_t enb_cc_idx, uint32_t mme_id, uint32_t enb_id, uint16_t rnti) override {}
  void log_sector_start(uint32_t cc_idx, uint32_t pci, uint32_t cell_id) override {}
  void log_sector_stop(uint32_t cc_idx, uint32_t pci, uint32_t cell_id) override {}
  void log_measurement_report(uint32_t           enb_cc_idx,
                              const std::string& asn1_oct_str,
                              const std::string& asn1_txt_str,
                              uint16_t           rnti) override
  {}
  void log_rlf_report(uint32_t           enb_cc_idx,
                      const std::string& asn1_oct_str,
                      const std::string& asn1_txt_str,
                      uint16_t           rnti) override
  {}
  void log_rlf_detected(uint32_t enb_cc_idx, const std::string& type, uint16_t rnti) override {}
  void log_handover_command(uint32_t enb_cc_idx,
                            uint32_t target_pci,
                            uint32_t target_earfcn,
                            uint16_t new_ue_rnti,
                            uint16_t rnti) override
  {}
  void log_connection_resume(uint32_t enb_cc_idx, uint16_t resume_rnti, uint16_t rnti) override {}
};

} // namespace

/// Returns the current time in seconds with ms precision since UNIX epoch.
static double get_time_stamp()
{
  auto tp = std::chrono::system_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::milliseconds>(tp).count() * 1e-3;
}

namespace {

/// Common metrics to all events.
DECLARE_METRIC("type", metric_type_tag, std::string, "");
DECLARE_METRIC("timestamp", metric_timestamp_tag, double, "");
DECLARE_METRIC("sector_id", metric_sector_id, uint32_t, "");
DECLARE_METRIC("cell_id", metric_cell_id, uint32_t, "");
DECLARE_METRIC("event_name", metric_event_name, std::string, "");

DECLARE_METRIC("rnti", metric_rnti, uint16_t, "");

/// ASN1 message metrics.
DECLARE_METRIC("asn1_length", metric_asn1_length, uint32_t, "");
DECLARE_METRIC("asn1_message", metric_asn1_message, std::string, "");

/// Context for sector start/stop.
DECLARE_METRIC("pci", metric_pci, uint32_t, "");
DECLARE_METRIC("cell_identity", metric_cell_identity, std::string, "");
DECLARE_METRIC("sib9_home_enb_name", metric_sib9_home_enb_name, std::string, "");
DECLARE_METRIC_SET("event_data", mset_sector_event, metric_pci, metric_cell_identity, metric_sib9_home_enb_name);
using sector_event_t = srslog::build_context_type<metric_type_tag,
                                                  metric_timestamp_tag,
                                                  metric_sector_id,
                                                  metric_cell_id,
                                                  metric_event_name,
                                                  mset_sector_event>;

/// Context for a RRC event.
DECLARE_METRIC("asn1_type", metric_asn1_type, uint32_t, "");
DECLARE_METRIC("additional", metric_additional, uint32_t, "");
DECLARE_METRIC_SET("event_data",
                   mset_rrc_event,
                   metric_rnti,
                   metric_asn1_length,
                   metric_asn1_message,
                   metric_asn1_type,
                   metric_additional);
using rrc_event_t = srslog::build_context_type<metric_type_tag,
                                               metric_timestamp_tag,
                                               metric_sector_id,
                                               metric_cell_id,
                                               metric_event_name,
                                               mset_rrc_event>;

/// Context for S1 context create/delete.
DECLARE_METRIC("mme_ue_s1ap_id", metric_ue_mme_id, uint32_t, "");
DECLARE_METRIC("enb_ue_s1ap_id", metric_ue_enb_id, uint32_t, "");
DECLARE_METRIC_SET("event_data", mset_s1apctx_event, metric_ue_mme_id, metric_ue_enb_id, metric_rnti);
using s1apctx_event_t = srslog::build_context_type<metric_type_tag,
                                                   metric_timestamp_tag,
                                                   metric_sector_id,
                                                   metric_cell_id,
                                                   metric_event_name,
                                                   mset_s1apctx_event>;

/// Context for the RLF report event.
DECLARE_METRIC_SET("event_data", mset_rlf_report_event, metric_asn1_length, metric_asn1_message, metric_rnti);
using rlf_report_event_t = srslog::build_context_type<metric_type_tag,
                                                      metric_timestamp_tag,
                                                      metric_sector_id,
                                                      metric_cell_id,
                                                      metric_event_name,
                                                      mset_rlf_report_event>;

/// Context for measurement report.
DECLARE_METRIC_SET("event_data", mset_meas_report_event, metric_asn1_length, metric_asn1_message, metric_rnti);
using meas_report_event_t = srslog::build_context_type<metric_type_tag,
                                                       metric_timestamp_tag,
                                                       metric_sector_id,
                                                       metric_cell_id,
                                                       metric_event_name,
                                                       mset_meas_report_event>;

/// Context for the handover command event.
DECLARE_METRIC("target_pci", metric_target_pci, uint32_t, "");
DECLARE_METRIC("target_earfcn", metric_target_earfcn, uint32_t, "");
DECLARE_METRIC("new_ue_rnti", metric_new_ue_rnti, uint32_t, "");
DECLARE_METRIC_SET("event_data",
                   mset_ho_cmd_event,
                   metric_rnti,
                   metric_target_pci,
                   metric_target_earfcn,
                   metric_new_ue_rnti);
using ho_cmd_t = srslog::build_context_type<metric_type_tag,
                                            metric_timestamp_tag,
                                            metric_sector_id,
                                            metric_cell_id,
                                            metric_event_name,
                                            mset_ho_cmd_event>;

/// Context for the connection resume event.
DECLARE_METRIC("resume_rnti", metric_resume_rnti, uint32_t, "");
DECLARE_METRIC_SET("event_data", mset_conn_resume_event, metric_rnti, metric_resume_rnti);
using conn_resume_t = srslog::build_context_type<metric_type_tag,
                                                 metric_timestamp_tag,
                                                 metric_sector_id,
                                                 metric_cell_id,
                                                 metric_event_name,
                                                 mset_conn_resume_event>;

/// Context for the RLF detected event.
DECLARE_METRIC("type", metric_rlf_type, std::string, "");
DECLARE_METRIC_SET("event_data", mset_rlf_detected_event, metric_rnti, metric_rlf_type);
using rlf_detected_t = srslog::build_context_type<metric_type_tag,
                                                  metric_timestamp_tag,
                                                  metric_sector_id,
                                                  metric_cell_id,
                                                  metric_event_name,
                                                  mset_rlf_detected_event>;

/// Logs events into the configured log channel.
class logging_event_logger : public event_logger_interface
{
public:
  logging_event_logger(srslog::log_channel& c, event_logger::asn1_output_format asn1_format) :
    event_channel(c), asn1_format(asn1_format)
  {}

  void log_rrc_event(uint32_t           enb_cc_idx,
                     const std::string& asn1_oct_str,
                     const std::string& asn1_txt_str,
                     unsigned           type,
                     unsigned           additional_info,
                     uint16_t           rnti) override
  {
    rrc_event_t ctx("");

    const std::string& asn1 = (asn1_format == event_logger::asn1_output_format::octets) ? asn1_oct_str : asn1_txt_str;

    ctx.write<metric_type_tag>("event");
    ctx.write<metric_timestamp_tag>(get_time_stamp());
    ctx.write<metric_sector_id>(enb_cc_idx);
    ctx.write<metric_cell_id>(get_pci(enb_cc_idx));
    ctx.write<metric_event_name>("rrc_log");
    ctx.get<mset_rrc_event>().write<metric_rnti>(rnti);
    ctx.get<mset_rrc_event>().write<metric_asn1_length>(asn1.size());
    ctx.get<mset_rrc_event>().write<metric_asn1_message>(asn1);
    ctx.get<mset_rrc_event>().write<metric_asn1_type>(type);
    ctx.get<mset_rrc_event>().write<metric_additional>(additional_info);
    event_channel(ctx);
  }

  void log_s1_ctx_create(uint32_t enb_cc_idx, uint32_t mme_id, uint32_t enb_id, uint16_t rnti) override
  {
    s1apctx_event_t ctx("");

    ctx.write<metric_type_tag>("event");
    ctx.write<metric_timestamp_tag>(get_time_stamp());
    ctx.write<metric_sector_id>(enb_cc_idx);
    ctx.write<metric_cell_id>(get_pci(enb_cc_idx));
    ctx.write<metric_event_name>("s1_context_create");
    ctx.get<mset_s1apctx_event>().write<metric_ue_mme_id>(mme_id);
    ctx.get<mset_s1apctx_event>().write<metric_ue_enb_id>(enb_id);
    ctx.get<mset_s1apctx_event>().write<metric_rnti>(rnti);
    event_channel(ctx);
  }

  void log_s1_ctx_delete(uint32_t enb_cc_idx, uint32_t mme_id, uint32_t enb_id, uint16_t rnti) override
  {
    s1apctx_event_t ctx("");

    ctx.write<metric_type_tag>("event");
    ctx.write<metric_timestamp_tag>(get_time_stamp());
    ctx.write<metric_sector_id>(enb_cc_idx);
    ctx.write<metric_cell_id>(get_pci(enb_cc_idx));
    ctx.write<metric_event_name>("s1_context_delete");
    ctx.get<mset_s1apctx_event>().write<metric_ue_mme_id>(mme_id);
    ctx.get<mset_s1apctx_event>().write<metric_ue_enb_id>(enb_id);
    ctx.get<mset_s1apctx_event>().write<metric_rnti>(rnti);
    event_channel(ctx);
  }

  void log_sector_start(uint32_t cc_idx, uint32_t pci, uint32_t cell_id) override
  {
    register_pci(cc_idx, pci);

    sector_event_t ctx("");

    ctx.write<metric_type_tag>("event");
    ctx.write<metric_timestamp_tag>(get_time_stamp());
    ctx.write<metric_sector_id>(cc_idx);
    ctx.write<metric_cell_id>(pci);
    ctx.write<metric_event_name>("sector_start");
    ctx.get<mset_sector_event>().write<metric_pci>(pci);
    ctx.get<mset_sector_event>().write<metric_cell_identity>(fmt::to_string(cell_id));
    ctx.get<mset_sector_event>().write<metric_sib9_home_enb_name>("TODO");
    event_channel(ctx);
  }

  void log_sector_stop(uint32_t cc_idx, uint32_t pci, uint32_t cell_id) override
  {
    sector_event_t ctx("");

    ctx.write<metric_type_tag>("event");
    ctx.write<metric_timestamp_tag>(get_time_stamp());
    ctx.write<metric_sector_id>(cc_idx);
    ctx.write<metric_cell_id>(get_pci(cc_idx));
    ctx.write<metric_event_name>("sector_stop");
    ctx.get<mset_sector_event>().write<metric_pci>(pci);
    ctx.get<mset_sector_event>().write<metric_cell_identity>(fmt::to_string(cell_id));
    ctx.get<mset_sector_event>().write<metric_sib9_home_enb_name>("TODO");
    event_channel(ctx);
  }

  void log_measurement_report(uint32_t           enb_cc_idx,
                              const std::string& asn1_oct_str,
                              const std::string& asn1_txt_str,
                              uint16_t           rnti) override
  {
    meas_report_event_t ctx("");

    const std::string& asn1 = (asn1_format == event_logger::asn1_output_format::octets) ? asn1_oct_str : asn1_txt_str;

    ctx.write<metric_type_tag>("event");
    ctx.write<metric_timestamp_tag>(get_time_stamp());
    ctx.write<metric_sector_id>(enb_cc_idx);
    ctx.write<metric_cell_id>(get_pci(enb_cc_idx));
    ctx.write<metric_event_name>("measurement_report");
    ctx.get<mset_meas_report_event>().write<metric_asn1_length>(asn1.size());
    ctx.get<mset_meas_report_event>().write<metric_asn1_message>(asn1);
    ctx.get<mset_meas_report_event>().write<metric_rnti>(rnti);
    event_channel(ctx);
  }

  void log_rlf_report(uint32_t           enb_cc_idx,
                      const std::string& asn1_oct_str,
                      const std::string& asn1_txt_str,
                      uint16_t           rnti) override
  {
    rlf_report_event_t ctx("");

    const std::string& asn1 = (asn1_format == event_logger::asn1_output_format::octets) ? asn1_oct_str : asn1_txt_str;

    ctx.write<metric_type_tag>("event");
    ctx.write<metric_timestamp_tag>(get_time_stamp());
    ctx.write<metric_sector_id>(enb_cc_idx);
    ctx.write<metric_cell_id>(get_pci(enb_cc_idx));
    ctx.write<metric_event_name>("rlf_report");
    ctx.get<mset_rlf_report_event>().write<metric_asn1_length>(asn1.size());
    ctx.get<mset_rlf_report_event>().write<metric_asn1_message>(asn1);
    ctx.get<mset_rlf_report_event>().write<metric_rnti>(rnti);
    event_channel(ctx);
  }

  void log_rlf_detected(uint32_t enb_cc_idx, const std::string& type, uint16_t rnti) override
  {
    rlf_detected_t ctx("");

    ctx.write<metric_type_tag>("event");
    ctx.write<metric_timestamp_tag>(get_time_stamp());
    ctx.write<metric_sector_id>(enb_cc_idx);
    ctx.write<metric_cell_id>(get_pci(enb_cc_idx));
    ctx.write<metric_event_name>("rlf_detected");
    ctx.get<mset_rlf_detected_event>().write<metric_rnti>(rnti);
    ctx.get<mset_rlf_detected_event>().write<metric_rlf_type>(type);
    event_channel(ctx);
  }

  void log_handover_command(uint32_t enb_cc_idx,
                            uint32_t target_pci,
                            uint32_t target_earfcn,
                            uint16_t new_ue_rnti,
                            uint16_t rnti) override
  {
    ho_cmd_t ctx("");

    ctx.write<metric_type_tag>("event");
    ctx.write<metric_timestamp_tag>(get_time_stamp());
    ctx.write<metric_sector_id>(enb_cc_idx);
    ctx.write<metric_cell_id>(get_pci(enb_cc_idx));
    ctx.write<metric_event_name>("ho_command");
    ctx.get<mset_ho_cmd_event>().write<metric_rnti>(rnti);
    ctx.get<mset_ho_cmd_event>().write<metric_target_pci>(target_pci);
    ctx.get<mset_ho_cmd_event>().write<metric_target_earfcn>(target_earfcn);
    ctx.get<mset_ho_cmd_event>().write<metric_new_ue_rnti>(new_ue_rnti);
    event_channel(ctx);
  }

  void log_connection_resume(uint32_t enb_cc_idx, uint16_t resume_rnti, uint16_t rnti) override
  {
    conn_resume_t ctx("");

    ctx.write<metric_type_tag>("event");
    ctx.write<metric_timestamp_tag>(get_time_stamp());
    ctx.write<metric_sector_id>(enb_cc_idx);
    ctx.write<metric_cell_id>(get_pci(enb_cc_idx));
    ctx.write<metric_event_name>("connection_resume");
    ctx.get<mset_conn_resume_event>().write<metric_rnti>(rnti);
    ctx.get<mset_conn_resume_event>().write<metric_resume_rnti>(resume_rnti);
    event_channel(ctx);
  }

private:
  /// Associates the corresponding cc_idx with its PCI.
  void register_pci(uint32_t cc_idx, uint32_t pci)
  {
    std::lock_guard<std::mutex> lock(m);
    cc_idx_to_pci_map[cc_idx] = pci;
  }

  /// Returns the PCI value associated to specified cc_idx, otherwise returns an invalid PCI value if not previous
  /// association has been set.
  uint32_t get_pci(uint32_t cc_idx) const
  {
    std::lock_guard<std::mutex> lock(m);
    auto                        it = cc_idx_to_pci_map.find(cc_idx);
    return (it != cc_idx_to_pci_map.cend()) ? it->second : 999;
  }

private:
  srslog::log_channel&                   event_channel;
  event_logger::asn1_output_format       asn1_format;
  std::unordered_map<uint32_t, uint32_t> cc_idx_to_pci_map;
  mutable std::mutex                     m;
};

} // namespace

std::unique_ptr<event_logger_interface> event_logger::pimpl = std::unique_ptr<null_event_logger>(new null_event_logger);

event_logger_interface& event_logger::get()
{
  return *pimpl;
}

void event_logger::configure(srslog::log_channel& c, asn1_output_format asn1_format)
{
  pimpl = std::unique_ptr<logging_event_logger>(new logging_event_logger(c, asn1_format));
}
