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

#include "srsran/common/enb_events.h"
#include "srsran/srslog/context.h"
#include "srsran/srslog/log_channel.h"

using namespace srsenb;

namespace {

/// Null object implementation that is used when no log channel is configured.
class null_event_logger : public event_logger_interface
{
public:
  void log_rrc_event(uint32_t           enb_cc_idx,
                     const std::string& asn1,
                     unsigned           type,
                     unsigned           additional_info,
                     uint16_t           rnti) override
  {}
  void log_s1_ctx_create(uint32_t enb_cc_idx, uint32_t mme_id, uint32_t enb_id, uint16_t rnti) override {}
  void log_s1_ctx_delete(uint32_t enb_cc_idx, uint32_t mme_id, uint32_t enb_id, uint16_t rnti) override {}
  void log_sector_start(uint32_t cc_idx, uint32_t pci, uint32_t cell_id) override {}
  void log_sector_stop(uint32_t cc_idx, uint32_t pci, uint32_t cell_id) override {}
  void log_measurement_report(uint32_t enb_cc_idx, const std::string& asn1, uint16_t rnti) override {}
  void log_rlf(uint32_t enb_cc_idx, const std::string& asn1, uint16_t rnti) override {}
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
using sector_event_t = srslog::
    build_context_type<metric_type_tag, metric_timestamp_tag, metric_sector_id, metric_event_name, mset_sector_event>;

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
using rrc_event_t = srslog::
    build_context_type<metric_type_tag, metric_timestamp_tag, metric_sector_id, metric_event_name, mset_rrc_event>;

/// Context for S1 context create/delete.
DECLARE_METRIC("mme_ue_s1ap_id", metric_ue_mme_id, uint32_t, "");
DECLARE_METRIC("enb_ue_s1ap_id", metric_ue_enb_id, uint32_t, "");
DECLARE_METRIC_SET("event_data", mset_s1apctx_event, metric_ue_mme_id, metric_ue_enb_id, metric_rnti);
using s1apctx_event_t = srslog::
    build_context_type<metric_type_tag, metric_timestamp_tag, metric_sector_id, metric_event_name, mset_s1apctx_event>;

/// Context for the RLF event.
DECLARE_METRIC_SET("event_data", mset_rlfctx_event, metric_asn1_length, metric_asn1_message, metric_rnti);
using rlfctx_event_t = srslog::
    build_context_type<metric_type_tag, metric_timestamp_tag, metric_sector_id, metric_event_name, mset_rlfctx_event>;

/// Context for measurement report.
DECLARE_METRIC_SET("event_data", mset_meas_report_event, metric_asn1_length, metric_asn1_message, metric_rnti);
using meas_report_event_t = srslog::build_context_type<metric_type_tag,
                                                       metric_timestamp_tag,
                                                       metric_sector_id,
                                                       metric_event_name,
                                                       mset_meas_report_event>;

/// Logs events into the configured log channel.
class logging_event_logger : public event_logger_interface
{
public:
  explicit logging_event_logger(srslog::log_channel& c) : event_channel(c) {}

  void log_rrc_event(uint32_t           enb_cc_idx,
                     const std::string& asn1,
                     unsigned           type,
                     unsigned           additional_info,
                     uint16_t           rnti) override
  {
    rrc_event_t ctx("");

    ctx.write<metric_type_tag>("event");
    ctx.write<metric_timestamp_tag>(get_time_stamp());
    ctx.write<metric_sector_id>(enb_cc_idx);
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
    ctx.write<metric_event_name>("s1_context_delete");
    ctx.get<mset_s1apctx_event>().write<metric_ue_mme_id>(mme_id);
    ctx.get<mset_s1apctx_event>().write<metric_ue_enb_id>(enb_id);
    ctx.get<mset_s1apctx_event>().write<metric_rnti>(rnti);
    event_channel(ctx);
  }

  void log_sector_start(uint32_t cc_idx, uint32_t pci, uint32_t cell_id) override
  {
    sector_event_t ctx("");

    ctx.write<metric_type_tag>("event");
    ctx.write<metric_timestamp_tag>(get_time_stamp());
    ctx.write<metric_sector_id>(cc_idx);
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
    ctx.write<metric_event_name>("sector_stop");
    ctx.get<mset_sector_event>().write<metric_pci>(pci);
    ctx.get<mset_sector_event>().write<metric_cell_identity>(fmt::to_string(cell_id));
    ctx.get<mset_sector_event>().write<metric_sib9_home_enb_name>("TODO");
    event_channel(ctx);
  }

  void log_measurement_report(uint32_t enb_cc_idx, const std::string& asn1, uint16_t rnti) override
  {
    meas_report_event_t ctx("");

    ctx.write<metric_type_tag>("event");
    ctx.write<metric_timestamp_tag>(get_time_stamp());
    ctx.write<metric_sector_id>(enb_cc_idx);
    ctx.write<metric_event_name>("measurement_report");
    ctx.get<mset_meas_report_event>().write<metric_asn1_length>(asn1.size());
    ctx.get<mset_meas_report_event>().write<metric_asn1_message>(asn1);
    ctx.get<mset_meas_report_event>().write<metric_rnti>(rnti);
    event_channel(ctx);
  }

  void log_rlf(uint32_t enb_cc_idx, const std::string& asn1, uint16_t rnti) override
  {
    rlfctx_event_t ctx("");

    ctx.write<metric_type_tag>("event");
    ctx.write<metric_timestamp_tag>(get_time_stamp());
    ctx.write<metric_sector_id>(enb_cc_idx);
    ctx.write<metric_event_name>("radio_link_failure");
    ctx.get<mset_rlfctx_event>().write<metric_asn1_length>(asn1.size());
    ctx.get<mset_rlfctx_event>().write<metric_asn1_message>(asn1);
    ctx.get<mset_rlfctx_event>().write<metric_rnti>(rnti);
    event_channel(ctx);
  }

private:
  srslog::log_channel& event_channel;
};

} // namespace

std::unique_ptr<event_logger_interface> event_logger::pimpl = std::unique_ptr<null_event_logger>(new null_event_logger);

event_logger_interface& event_logger::get()
{
  return *pimpl;
}

void event_logger::configure(srslog::log_channel& c)
{
  pimpl = std::unique_ptr<logging_event_logger>(new logging_event_logger(c));
}
