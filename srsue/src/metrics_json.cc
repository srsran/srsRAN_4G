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

#include "srsue/hdr/metrics_json.h"
#include "srsran/srslog/context.h"

using namespace srsue;

void metrics_json::set_ue_handle(ue_metrics_interface* ue_)
{
  std::lock_guard<std::mutex> lock(mutex);
  ue = ue_;
}

namespace {

/// Shared metrics between containers.
DECLARE_METRIC("pci", metric_pci, uint32_t, "");
DECLARE_METRIC("dl_bitrate", metric_dl_brate, float, "");
DECLARE_METRIC("ul_bitrate", metric_ul_brate, float, "");
DECLARE_METRIC("rsrp", metric_rsrp, float, "");

/// MAC container.
DECLARE_METRIC("dl_bler", metric_dl_bler, float, "");
DECLARE_METRIC("ul_bler", metric_ul_bler, float, "");
DECLARE_METRIC("ul_buff", metric_ul_buff, uint32_t, "");
DECLARE_METRIC_SET("mac_container",
                   mset_mac_container,
                   metric_dl_brate,
                   metric_dl_bler,
                   metric_ul_brate,
                   metric_ul_bler,
                   metric_ul_buff);

/// Carrier container.
DECLARE_METRIC("earfcn", metric_earfcn, uint32_t, "");
DECLARE_METRIC("pathloss", metric_pathloss, float, "");
DECLARE_METRIC("cfo", metric_cfo, float, "");
DECLARE_METRIC("dl_snr", metric_dl_snr, float, "");
DECLARE_METRIC("dl_mcs", metric_dl_mcs, float, "");
DECLARE_METRIC("ul_mcs", metric_ul_mcs, float, "");
DECLARE_METRIC("ul_ta", metric_ul_ta, float, "");
DECLARE_METRIC("distance_km", metric_distance_km, float, "");
DECLARE_METRIC("speed_kmph", metric_speed_kmph, float, "");
DECLARE_METRIC_SET("carrier_container",
                   mset_carrier_container,
                   metric_earfcn,
                   metric_pci,
                   metric_rsrp,
                   metric_pathloss,
                   metric_cfo,
                   metric_dl_snr,
                   metric_dl_mcs,
                   metric_ul_mcs,
                   metric_ul_ta,
                   metric_distance_km,
                   metric_speed_kmph,
                   mset_mac_container);
DECLARE_METRIC_LIST("carrier_list", mlist_carriers, std::vector<mset_carrier_container>);

/// GW container.
DECLARE_METRIC_SET("gw_container", mset_gw_container, metric_dl_brate, metric_ul_brate);

/// RRC container.
DECLARE_METRIC("rrc_state", metric_rrc_state, std::string, "");
DECLARE_METRIC_SET("rrc_container", mset_rrc_container, metric_rrc_state);

/// Neighbour cell list.
DECLARE_METRIC_SET("neighbour_cell_container", mset_neighbour_cell_container, metric_pci, metric_rsrp, metric_cfo);
DECLARE_METRIC_LIST("neighbour_cell_list", mlist_neighbours, std::vector<mset_neighbour_cell_container>);

/// NAS container.
DECLARE_METRIC("emm_state", metric_emm_state, std::string, "");
DECLARE_METRIC_SET("nas_container", mset_nas_container, metric_emm_state);

/// RF container.
DECLARE_METRIC("rf_o", metric_rf_o, uint32_t, "");
DECLARE_METRIC("rf_u", metric_rf_u, uint32_t, "");
DECLARE_METRIC("rf_l", metric_rf_l, uint32_t, "");
DECLARE_METRIC_SET("rf_container", mset_rf_container, metric_rf_o, metric_rf_u, metric_rf_l);

/// System memory container.
DECLARE_METRIC("proc_realmem_percent", metric_proc_rmem_percent, uint32_t, "");
DECLARE_METRIC("proc_realmem_kB", metric_proc_rmem_kB, uint32_t, "");
DECLARE_METRIC("proc_vmem_kB", metric_proc_vmem_kB, uint32_t, "");
DECLARE_METRIC("sys_mem_usage_percent", metric_sys_mem_percent, uint32_t, "");
DECLARE_METRIC_SET("sys_memory_container",
                   mset_sys_mem_container,
                   metric_proc_rmem_percent,
                   metric_proc_rmem_kB,
                   metric_proc_vmem_kB,
                   metric_sys_mem_percent);

/// System CPU container
DECLARE_METRIC("proc_cpu_usage", metric_proc_cpu_usage, uint32_t, "");
DECLARE_METRIC("proc_thread_count", metric_thread_count, uint32_t, "");
DECLARE_METRIC("sys_core_usage", metric_proc_core_usage, uint32_t, "");
DECLARE_METRIC_SET("cpu_core_container", mset_cpu_core_container, metric_proc_core_usage);
DECLARE_METRIC_LIST("cpu_core_list", mlist_cpu_core_list, std::vector<mset_cpu_core_container>);
DECLARE_METRIC_SET("sys_cpu_container",
                   mset_sys_cpu_container,
                   metric_proc_cpu_usage,
                   metric_thread_count,
                   mlist_cpu_core_list);

/// Metrics root object.
DECLARE_METRIC("type", metric_type_tag, std::string, "");
DECLARE_METRIC("timestamp", metric_timestamp_tag, double, "");

/// Metrics context.
using metric_context_t = srslog::build_context_type<metric_type_tag,
                                                    metric_timestamp_tag,
                                                    mlist_carriers,
                                                    mset_gw_container,
                                                    mset_rrc_container,
                                                    mlist_neighbours,
                                                    mset_nas_container,
                                                    mset_rf_container,
                                                    mset_sys_mem_container,
                                                    mset_sys_cpu_container>;

} // namespace

/// Returns the current time in seconds with ms precision since UNIX epoch.
static double get_time_stamp()
{
  auto tp = std::chrono::system_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::milliseconds>(tp).count() * 1e-3;
}

void metrics_json::set_metrics(const ue_metrics_t& metrics, const uint32_t period_usec)
{
  std::lock_guard<std::mutex> lock(mutex);
  if (!ue) {
    return;
  }

  metric_context_t ctx("JSON Metrics");

  // Fill root object.
  ctx.write<metric_type_tag>("metrics");

  // Fill cc container.
  auto& carrier_list = ctx.get<mlist_carriers>();
  carrier_list.resize(metrics.phy.nof_active_cc);
  for (uint32_t i = 0, e = carrier_list.size(); i != e; ++i) {
    if (metrics.stack.mac[i].nof_tti == 0) {
      continue;
    }

    auto& carrier = carrier_list[i];

    // PHY.
    carrier.write<metric_earfcn>(metrics.phy.info[i].dl_earfcn);
    carrier.write<metric_pci>(metrics.phy.info[i].pci);

    carrier.write<metric_rsrp>(metrics.phy.ch[i].rsrp);
    carrier.write<metric_pathloss>(metrics.phy.ch[i].pathloss);

    carrier.write<metric_cfo>(metrics.phy.sync[i].cfo);

    carrier.write<metric_dl_snr>(metrics.phy.ch[i].sinr);
    carrier.write<metric_dl_mcs>(metrics.phy.dl[i].mcs);
    carrier.write<metric_ul_mcs>(metrics.phy.ul[i].mcs);
    carrier.write<metric_ul_ta>(metrics.phy.sync[i].ta_us);
    carrier.write<metric_distance_km>(metrics.phy.sync[i].distance_km);
    carrier.write<metric_speed_kmph>(metrics.phy.sync[i].speed_kmph);

    // MAC
    carrier.get<mset_mac_container>().write<metric_dl_brate>(metrics.stack.mac[i].rx_brate /
                                                             metrics.stack.mac[i].nof_tti * 1e-3);
    carrier.get<mset_mac_container>().write<metric_dl_bler>(
        (metrics.stack.mac[i].rx_pkts > 0) ? (float)100 * metrics.stack.mac[i].rx_errors / metrics.stack.mac[i].rx_pkts
                                           : 0);
    carrier.get<mset_mac_container>().write<metric_ul_brate>(metrics.stack.mac[i].tx_brate /
                                                             metrics.stack.mac[i].nof_tti * 1e-3);
    carrier.get<mset_mac_container>().write<metric_ul_bler>(
        (metrics.stack.mac[i].tx_pkts > 0) ? (float)100 * metrics.stack.mac[i].tx_errors / metrics.stack.mac[i].tx_pkts
                                           : 0);
    carrier.get<mset_mac_container>().write<metric_ul_buff>(metrics.stack.mac[i].ul_buffer);
  }

  // Fill GW container.
  ctx.get<mset_gw_container>().write<metric_dl_brate>(metrics.gw.dl_tput_mbps);
  ctx.get<mset_gw_container>().write<metric_ul_brate>(metrics.gw.ul_tput_mbps);

  // Fill RRC container.
  ctx.get<mset_rrc_container>().write<metric_rrc_state>(rrc_state_text[metrics.stack.rrc.state]);

  // Fill neighbour list.
  auto& neighbour_list = ctx.get<mlist_neighbours>();
  neighbour_list.resize(metrics.stack.rrc.neighbour_cells.size());
  for (uint32_t i = 0, e = neighbour_list.size(); i != e; ++i) {
    auto& neigbour = neighbour_list[i];
    neigbour.write<metric_pci>(metrics.stack.rrc.neighbour_cells[i].pci);
    neigbour.write<metric_rsrp>(metrics.stack.rrc.neighbour_cells[i].rsrp);
    neigbour.write<metric_cfo>(metrics.stack.rrc.neighbour_cells[i].cfo_hz);
  }

  // Fill NAS container.
  ctx.get<mset_nas_container>().write<metric_emm_state>(emm_state_text(metrics.stack.nas.state));

  // Fill RF container.
  ctx.get<mset_rf_container>().write<metric_rf_o>(metrics.rf.rf_o);
  ctx.get<mset_rf_container>().write<metric_rf_u>(metrics.rf.rf_u);
  ctx.get<mset_rf_container>().write<metric_rf_l>(metrics.rf.rf_l);

  // Fill system memory container.
  ctx.get<mset_sys_mem_container>().write<metric_proc_rmem_percent>(metrics.sys.process_realmem);
  ctx.get<mset_sys_mem_container>().write<metric_proc_rmem_kB>(metrics.sys.process_realmem_kB);
  ctx.get<mset_sys_mem_container>().write<metric_proc_vmem_kB>(metrics.sys.process_virtualmem_kB);
  ctx.get<mset_sys_mem_container>().write<metric_sys_mem_percent>(metrics.sys.system_mem);

  // Fill system CPU container.
  ctx.get<mset_sys_cpu_container>().write<metric_proc_cpu_usage>(metrics.sys.process_cpu_usage);
  ctx.get<mset_sys_cpu_container>().write<metric_thread_count>(metrics.sys.thread_count);
  auto& core_list = ctx.get<mset_sys_cpu_container>().get<mlist_cpu_core_list>();
  core_list.resize(metrics.sys.cpu_count);
  for (uint32_t i = 0, e = core_list.size(); i != e; ++i) {
    core_list[i].write<metric_proc_core_usage>(metrics.sys.cpu_load[i]);
  }

  // Log the context.
  ctx.write<metric_timestamp_tag>(get_time_stamp());
  log_c(ctx);
}
