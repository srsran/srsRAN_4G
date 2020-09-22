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

#ifndef SRSUE_TTCN3_SYS_INTERFACE_H
#define SRSUE_TTCN3_SYS_INTERFACE_H

#include "srslte/asn1/rrc_asn1_utils.h"
#include "srslte/common/buffer_pool.h"
#include "ttcn3_helpers.h"
#include "ttcn3_interfaces.h"

using namespace srslte;

// The EUTRA.SYS interface
class ttcn3_sys_interface : public ttcn3_port_handler
{
public:
  ttcn3_sys_interface(){};
  ~ttcn3_sys_interface(){};

  int init(ss_sys_interface* syssim_, srslte::log* log_, std::string net_ip_, uint32_t net_port_)
  {
    syssim      = syssim_;
    net_ip      = net_ip_;
    net_port    = net_port_;
    log         = log_;
    initialized = true;
    log->debug("Initialized.\n");
    pool = byte_buffer_pool::get_instance();
    return port_listen();
  }

private:
  ///< Main message handler
  int handle_message(const unique_byte_array_t& rx_buf, const uint32_t n)
  {
    log->debug("Received %d B from remote.\n", n);

    // Chop incoming msg, first two bytes are length of the JSON
    // (see IPL4_EUTRA_SYSTEM_Definitions.ttcn
    uint16_t json_len = ((uint16_t)rx_buf->at(0) << 8) | rx_buf->at(1);

    // Copy JSON from received buffer and null-terminate
    char json[json_len + 1];
    memcpy(json, &rx_buf->at(2), json_len);
    json[json_len] = '\0';

    // The data part after the JSON starts right here but handling
    // is done in the respective functions
    uint16_t rx_buf_offset = json_len + 2;

    Document document;
    if (document.Parse(json).HasParseError() || document.IsObject() == false) {
      log->error_hex((uint8*)json, json_len, "Error parsing incoming data.\n");
      return SRSLTE_ERROR;
    }

    // Pretty-print
    StringBuffer               buffer;
    PrettyWriter<StringBuffer> writer(buffer);
    document.Accept(writer);
    log->info_long("Received %d bytes\n%s\n", json_len, (char*)buffer.GetString());

    // check for common
    assert(document.HasMember("Common"));
    assert(document["Common"].IsObject());

    // Check for request type
    assert(document.HasMember("Request"));
    assert(document["Request"].IsObject());

    // Get request type
    const Value& request = document["Request"];
    if (request.HasMember("Cell")) {
      log->info("Received Cell request.\n");
      handle_request_cell(document, &rx_buf->at(rx_buf_offset), n - rx_buf_offset);
    } else if (request.HasMember("L1MacIndCtrl")) {
      log->info("Received L1MacIndCtrl request.\n");
      handle_request_l1_mac_ind_ctrl(document);
    } else if (request.HasMember("RadioBearerList")) {
      log->info("Received RadioBearerList request.\n");
      handle_request_radio_bearer_list(document);
    } else if (request.HasMember("CellAttenuationList")) {
      log->info("Received CellAttenuationList request.\n");
      handle_request_cell_attenuation_list(document);
    } else if (request.HasMember("PdcpCount")) {
      log->info("Received PdcpCount request.\n");
      handle_request_pdcp_count(document);
    } else if (request.HasMember("AS_Security")) {
      log->info("Received AS_Security request.\n");
      handle_request_as_security(document);
    } else if (request.HasMember("EnquireTiming")) {
      log->info("Received EnquireTiming request.\n");
      handle_request_enquire_timing(document);
    } else if (request.HasMember("Paging")) {
      log->info("Received Paging request.\n");
      handle_request_paging(document, &rx_buf->at(rx_buf_offset), n - rx_buf_offset);
    } else if (request.HasMember("PdcpHandoverControl")) {
      log->info("Received PdcpHandoverControl.\n");
      handle_request_pdcp_handover_control(document);
    } else {
      log->error("Received unknown request.\n");
    }

    return SRSLTE_SUCCESS;
  }

  void handle_request_cell_basic(Document& document, const uint8_t* payload, const uint16_t len)
  {
    // That is the cellId or name that the testsuites uses to refer to a specific cell
    const Value& cell_name = document["Common"]["CellId"];
    assert(cell_name.IsString());

    if (document["Request"]["Cell"]["AddOrReconfigure"]["Basic"].HasMember("StaticCellInfo")) {
      // Fill all relevant info from the cell request
      ss_sys_interface::cell_config_t cell;

      // set name
      cell.name = cell_name.GetString();

      // Extract EARFCN
      const Value& earfcn =
          document["Request"]["Cell"]["AddOrReconfigure"]["Basic"]["StaticCellInfo"]["Downlink"]["Earfcn"];
      assert(earfcn.IsInt());
      cell.earfcn = earfcn.GetInt();

      // Extract cell config
      const Value& common_config = document["Request"]["Cell"]["AddOrReconfigure"]["Basic"]["StaticCellInfo"]["Common"];
      const Value& dl_config = document["Request"]["Cell"]["AddOrReconfigure"]["Basic"]["StaticCellInfo"]["Downlink"];
      const Value& phy_dl_config = document["Request"]["Cell"]["AddOrReconfigure"]["Basic"]["PhysicalLayerConfigDL"];

      cell.phy_cell.id = common_config["PhysicalCellId"].GetInt();
      cell.phy_cell.cp =
          (strcmp(dl_config["CyclicPrefix"].GetString(), "normal") == 0) ? SRSLTE_CP_NORM : SRSLTE_CP_EXT;
      cell.phy_cell.nof_ports =
          (strcmp(phy_dl_config["AntennaGroup"]["AntennaInfoCommon"]["R8"]["antennaPortsCount"].GetString(), "an1") ==
           0)
              ? 1
              : 2;
      cell.phy_cell.nof_prb = (strcmp(dl_config["Bandwidth"].GetString(), "n25") == 0) ? 25 : 0;
      cell.phy_cell.phich_length =
          (strcmp(phy_dl_config["Phich"]["PhichConfig"]["R8"]["phich_Duration"].GetString(), "normal") == 0)
              ? SRSLTE_PHICH_NORM
              : SRSLTE_PHICH_EXT;
      cell.phy_cell.phich_resources =
          (strcmp(phy_dl_config["Phich"]["PhichConfig"]["R8"]["phich_Resource"].GetString(), "one") == 0)
              ? SRSLTE_PHICH_R_1
              : SRSLTE_PHICH_R_1_6;
      log->info("DL EARFCN is %d with n_prb=%d\n", cell.earfcn, cell.phy_cell.nof_prb);

      const Value& ref_power =
          document["Request"]["Cell"]["AddOrReconfigure"]["Basic"]["InitialCellPower"]["MaxReferencePower"];
      assert(ref_power.IsInt());

      // set power
      cell.initial_power = ref_power.GetInt();

      const Value& att = document["Request"]["Cell"]["AddOrReconfigure"]["Basic"]["InitialCellPower"]["Attenuation"];

      cell.attenuation = 0;
      if (att.HasMember("Value")) {
        cell.attenuation = att["Value"].GetInt();
      } else if (att.HasMember("Off")) {
        // is there other values than Off=True?
        assert(att["Off"].GetBool() == true);
        if (att["Off"].GetBool() == true) {
          // use high attenuation value  (-145dB RX power as per TS 36.508 Sec 6.2.2.1-1 is a non-suitable Off cell)
          cell.attenuation = 90.0;
        }
      }

      // parse and handle reconfig of active cells
      handle_active_cell_reconfig_section(document, cell);

      // Now configure cell
      syssim->set_cell_config(ttcn3_helpers::get_timing_info(document), cell);
      log->info("Configuring attenuation of %s to %.2f dB\n", cell_name.GetString(), cell.attenuation);
      syssim->set_cell_attenuation(ttcn3_helpers::get_timing_info(document), cell_name.GetString(), cell.attenuation);
    }

    // Pull out SIBs and send to syssim
    uint16_t       consumed_bytes = 0;
    const uint8_t* payload_ptr    = payload;
    while (consumed_bytes < len) {
      uint16_t tb_len = ((uint16_t)payload_ptr[0] << 8) | payload_ptr[1];
      payload_ptr += 2;

      unique_byte_buffer_t sib = pool_allocate_blocking;
      memcpy(sib->msg, payload_ptr, tb_len);
      payload_ptr += tb_len;
      sib->N_bytes = tb_len;

      // Push to main component
      log->info_hex(sib->msg, sib->N_bytes, "Received BCCH DL-SCH for %s\n", cell_name.GetString());
      syssim->add_bcch_dlsch_pdu(cell_name.GetString(), std::move(sib));

      consumed_bytes = payload_ptr - payload;
    }

    if (document["Request"]["Cell"]["AddOrReconfigure"]["Basic"].HasMember("StaticCellInfo")) {
      // Create response for template car_CellConfig_CNF(CellId_Type p_CellId)
      std::string resp = ttcn3_helpers::get_basic_sys_req_cnf(cell_name.GetString(), "Cell");

      log->info("Sending %s to tester (%zd B)\n", resp.c_str(), resp.length());
      send((const uint8_t*)resp.c_str(), resp.length());
    }
  }

  void handle_request_cell_active(Document& document, const uint8_t* payload, const uint16_t len)
  {
    // Create response for template car_CellConfig_CNF(CellId_Type p_CellId)
    std::string cell_id = document["Common"]["CellId"].GetString();

    // Fill relevant content
    ss_sys_interface::cell_config_t cell;
    cell.name = cell_id;

    handle_active_cell_reconfig_section(document, cell);

    // Now configure cell
    syssim->set_cell_config(ttcn3_helpers::get_timing_info(document), cell);

    if (ttcn3_helpers::requires_confirm(document)) {
      std::string resp = ttcn3_helpers::get_basic_sys_req_cnf(cell_id, "Cell");

      log->info("Sending %s to tester (%zd B)\n", resp.c_str(), resp.length());
      send((const uint8_t*)resp.c_str(), resp.length());
    } else {
      log->info("Skipping response for request cell active message.\n");
    }
  }

  // This function just pulls out the reconfiguration section but doesn't send response to SS
  void handle_active_cell_reconfig_section(Document& document, ss_sys_interface::cell_config_t& cell)
  {
    if (document["Request"]["Cell"]["AddOrReconfigure"].HasMember("Active")) {
      // Extract CRNTI
      if (document["Request"]["Cell"]["AddOrReconfigure"]["Active"].HasMember("C_RNTI")) {
        const Value& crnti_string = document["Request"]["Cell"]["AddOrReconfigure"]["Active"]["C_RNTI"];
        assert(crnti_string.IsString());
        cell.crnti = std::bitset<16>(crnti_string.GetString()).to_ulong();
      }

      // Extra Cont Resolution scheme
      if (document["Request"]["Cell"]["AddOrReconfigure"]["Active"].HasMember("RachProcedureConfig")) {
        if (document["Request"]["Cell"]["AddOrReconfigure"]["Active"]["RachProcedureConfig"].HasMember(
                "RachProcedureList")) {
          const Value& rach_proc_list =
              document["Request"]["Cell"]["AddOrReconfigure"]["Active"]["RachProcedureConfig"]["RachProcedureList"];
          assert(rach_proc_list.IsArray());
          for (Value::ConstValueIterator itr = rach_proc_list.Begin(); itr != rach_proc_list.End(); ++itr) {
            if (itr->HasMember("ContentionResolutionCtrl")) {
              const Value& cont_res_type = (*itr)["ContentionResolutionCtrl"];
              if (cont_res_type.HasMember("CRNTI_Based")) {
                // TODO: handle CRNTI based contention resolution
              } else if (cont_res_type.HasMember("TCRNTI_Based")) {
                // TODO: handle TCRNTI based contention resolution
              }
            }
            if (itr->HasMember("RAResponse")) {
              if ((*itr)["RAResponse"].HasMember("Ctrl")) {
                if ((*itr)["RAResponse"]["Ctrl"].HasMember("Rar")) {
                  if ((*itr)["RAResponse"]["Ctrl"]["Rar"].HasMember("List")) {
                    const Value& rar_list_list = (*itr)["RAResponse"]["Ctrl"]["Rar"]["List"];
                    assert(rar_list_list.IsArray());
                    for (Value::ConstValueIterator rar_itr = rar_list_list.Begin(); rar_itr != rar_list_list.End();
                         ++rar_itr) {
                      if (rar_itr->HasMember("TempC_RNTI")) {
                        if ((*rar_itr)["TempC_RNTI"].HasMember("SameAsC_RNTI")) {
                          const Value& temp_crnti = (*rar_itr)["TempC_RNTI"]["SameAsC_RNTI"];
                          assert(temp_crnti.IsBool() && temp_crnti.GetBool() == true);
                          cell.temp_crnti = cell.crnti;
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  void handle_request_cell(Document& document, const uint8_t* payload, const uint16_t len)
  {
    // get Cmd and make sure it has MMI
    const Value& a = document["Common"];

    assert(a.HasMember("CellId"));

    // check cnf flag
    assert(a.HasMember("ControlInfo"));
    const Value& b = a["ControlInfo"];
    assert(b.HasMember("CnfFlag"));

    // Handle cell creation
    if (document["Request"]["Cell"].HasMember("AddOrReconfigure")) {
      if (document["Request"]["Cell"]["AddOrReconfigure"].HasMember("Basic")) {
        // basic information for a cell (e.g. broadcasting)
        handle_request_cell_basic(document, payload, len);
      } else if (document["Request"]["Cell"]["AddOrReconfigure"].HasMember("Active")) {
        // additional configuration for active cell (i.e. cell being capable to receive RACH preamble)
        handle_request_cell_active(document, payload, len);
      }
    } else if (document["Request"]["Cell"].HasMember("Release")) {
      log->info("Received cell release command\n");
      // do nothing more
    }
  }

  void handle_request_l1_mac_ind_ctrl(Document& document)
  {
    const Value& a = document["Common"];

    assert(a.HasMember("CellId"));
    const Value& cell_id = a["CellId"];

    // check cnf flag
    assert(a.HasMember("ControlInfo"));
    const Value& b = a["ControlInfo"];
    assert(b.HasMember("CnfFlag"));

    // check request
    const Value& req = document["Request"];
    assert(req.HasMember("L1MacIndCtrl"));

    const Value& mac_ind_ctrl = req["L1MacIndCtrl"];

    if (mac_ind_ctrl.HasMember("HarqError")) {
      assert(mac_ind_ctrl["HarqError"].IsString());
      bool harq_error = (strcmp(mac_ind_ctrl["HarqError"].GetString(), "enable") == 0) ? true : false;
      log->info("Setting HarqError to %s\n", harq_error ? "True" : "False");
    }

    if (ttcn3_helpers::requires_confirm(document)) {
      std::string resp = ttcn3_helpers::get_basic_sys_req_cnf(cell_id.GetString(), "L1MacIndCtrl");

      log->info("Sending %s to tester (%zd B)\n", resp.c_str(), resp.length());
      send((const uint8_t*)resp.c_str(), resp.length());
    }
  }

  void handle_request_radio_bearer_list(Document& document)
  {
    const Value& a = document["Common"];

    assert(a.HasMember("CellId"));
    const Value& cell_id = a["CellId"];

    // check cnf flag
    assert(a.HasMember("ControlInfo"));
    const Value& b = a["ControlInfo"];
    assert(b.HasMember("CnfFlag"));

    // check request
    const Value& req = document["Request"];
    assert(req.HasMember("RadioBearerList"));

    const Value& bearers = req["RadioBearerList"];
    assert(bearers.IsArray());

    // iterate over all bearers and configure them
    for (Value::ConstValueIterator itr = bearers.Begin(); itr != bearers.End(); ++itr) {
      assert(itr->HasMember("Id"));
      const Value& id = (*itr)["Id"];
      if (id.HasMember("Srb")) {
        const Value& config = (*itr)["Config"];
        if (config.HasMember("AddOrReconfigure")) {
          uint32_t     lcid = id["Srb"].GetInt();
          const Value& aor  = config["AddOrReconfigure"];
          if (aor.HasMember("Mac") && aor["Mac"].HasMember("TestMode") && aor["Mac"]["TestMode"].HasMember("Info") &&
              aor["Mac"]["TestMode"]["Info"].HasMember("DiffLogChId")) {
            uint32_t     force_lcid = 0;
            const Value& dlcid      = aor["Mac"]["TestMode"]["Info"]["DiffLogChId"];
            assert(dlcid.HasMember("LogChId"));
            force_lcid = dlcid["LogChId"].GetInt();
            log->info("TestMode: lcid overridden: %d\n", force_lcid);
            syssim->set_forced_lcid(force_lcid);
          } else {
            // Unset override function to use different lcid
            log->info("TestMode: lcid reset\n");
            syssim->set_forced_lcid(-1);
          }
          if (lcid > 0) {
            pdcp_config_t pdcp_cfg = make_srb_pdcp_config_t(static_cast<uint8_t>(lcid), false);
            syssim->add_srb(
                ttcn3_helpers::get_timing_info(document), ttcn3_helpers::get_cell_name(document), lcid, pdcp_cfg);
          }
        } else if (config.HasMember("Release")) {
          uint32_t lcid = id["Srb"].GetInt();
          syssim->del_srb(ttcn3_helpers::get_timing_info(document), ttcn3_helpers::get_cell_name(document), lcid);
        } else {
          log->error("Unknown config.\n");
        }
      } else if (id.HasMember("Drb")) {
        log->info("Configure DRB%d\n", id["Drb"].GetInt());

        const Value& config = (*itr)["Config"];
        if (config.HasMember("AddOrReconfigure")) {
          const Value& aor = config["AddOrReconfigure"];
          if (aor.HasMember("LogicalChannelId")) {
            uint32_t lcid = aor["LogicalChannelId"].GetInt();
            if (lcid > 0) {
              pdcp_config_t pdcp_cfg = make_drb_pdcp_config_t(static_cast<uint8_t>(lcid), false);
              syssim->add_drb(
                  ttcn3_helpers::get_timing_info(document), ttcn3_helpers::get_cell_name(document), lcid, pdcp_cfg);
            }
          }
        } else if (config.HasMember("Release")) {
          uint32_t lcid = id["Drb"].GetInt() + 2;
          syssim->del_drb(ttcn3_helpers::get_timing_info(document), ttcn3_helpers::get_cell_name(document), lcid);
        } else {
          log->error("Unknown config.\n");
        }
      }
    }

    if (ttcn3_helpers::requires_confirm(document)) {
      std::string resp = ttcn3_helpers::get_basic_sys_req_cnf(cell_id.GetString(), "RadioBearerList");

      log->info("Sending %s to tester (%zd B)\n", resp.c_str(), resp.length());
      send((const uint8_t*)resp.c_str(), resp.length());
    } else {
      log->info("Skipping response for radio bearer list message.\n");
    }
  }

  void handle_request_cell_attenuation_list(Document& document)
  {
    const Value& a = document["Common"];

    assert(a.HasMember("CellId"));
    const Value& cell_id = a["CellId"];

    // check cnf flag
    assert(a.HasMember("ControlInfo"));
    const Value& b = a["ControlInfo"];
    assert(b.HasMember("CnfFlag"));

    // check request
    const Value& req = document["Request"];
    assert(req.HasMember("CellAttenuationList"));

    const Value& cells = req["CellAttenuationList"];
    assert(cells.IsArray());

    // iterate over all bearers and configure them
    for (Value::ConstValueIterator itr = cells.Begin(); itr != cells.End(); ++itr) {
      assert(itr->HasMember("CellId"));
      const Value& id = (*itr)["CellId"];

      assert(itr->HasMember("Attenuation"));
      const Value& att = (*itr)["Attenuation"];

      float att_value = 0;
      if (att.HasMember("Value")) {
        att_value = att["Value"].GetInt();
      } else if (att.HasMember("Off")) {
        // is there other values than Off=True?
        assert(att["Off"].GetBool() == true);
        if (att["Off"].GetBool() == true) {
          // use high attenuation value  (-145dB RX power as per TS 36.508 Sec 6.2.2.1-1 is a non-suitable Off cell)
          att_value = 90.0;
        }
      }

      log->info("Configuring attenuation of %s to %.2f dB\n", id.GetString(), att_value);
      syssim->set_cell_attenuation(ttcn3_helpers::get_timing_info(document), id.GetString(), att_value);
    }

    std::string resp = ttcn3_helpers::get_basic_sys_req_cnf(cell_id.GetString(), "CellAttenuationList");

    log->info("Sending %s to tester (%zd B)\n", resp.c_str(), resp.length());
    send((const uint8_t*)resp.c_str(), resp.length());
  }

  void handle_request_pdcp_count(Document& document)
  {
    const Value& a = document["Common"];

    assert(a.HasMember("CellId"));
    const Value& cell_id = a["CellId"];

    // check cnf flag
    assert(a.HasMember("ControlInfo"));
    const Value& b = a["ControlInfo"];
    assert(b.HasMember("CnfFlag"));

    // check request
    const Value& req = document["Request"];
    assert(req.HasMember("PdcpCount"));

    const Value& pdcp_count = req["PdcpCount"];
    assert(pdcp_count.HasMember("Get"));

    const Value& get = pdcp_count["Get"];
    assert(get.HasMember("AllRBs"));

    std::string resp = ttcn3_helpers::get_pdcp_count_response(
        cell_id.GetString(), syssim->get_pdcp_count(ttcn3_helpers::get_cell_name(document)));

    log->info("Sending %s to tester (%zd B)\n", resp.c_str(), resp.length());
    send((const uint8_t*)resp.c_str(), resp.length());
  }

  void handle_request_as_security(Document& document)
  {
    const Value& a = document["Common"];

    assert(a.HasMember("CellId"));
    const Value& cell_id = a["CellId"];

    // check cnf flag
    assert(a.HasMember("ControlInfo"));
    const Value& b = a["ControlInfo"];
    assert(b.HasMember("CnfFlag"));

    const Value& config_flag = b["CnfFlag"];
    assert(config_flag.IsBool());

    // check request
    const Value& req = document["Request"];
    assert(req.HasMember("AS_Security"));

    // check AS security start
    const Value& as_sec = req["AS_Security"];
    if (as_sec.HasMember("StartRestart")) {
      // get integrity algo
      srslte::INTEGRITY_ALGORITHM_ID_ENUM integ_algo = {};
      std::string int_algo_string                    = as_sec["StartRestart"]["Integrity"]["Algorithm"].GetString();
      if (int_algo_string == "eia0") {
        integ_algo = srslte::INTEGRITY_ALGORITHM_ID_EIA0;
      } else if (int_algo_string == "eia1") {
        integ_algo = srslte::INTEGRITY_ALGORITHM_ID_128_EIA1;
      } else if (int_algo_string == "eia2") {
        integ_algo = srslte::INTEGRITY_ALGORITHM_ID_128_EIA2;
      } else {
        log->error("Unsupported integrity algorithm %s\n", int_algo_string.c_str());
      }

      // get integrity key
      std::string             integ_key_string = as_sec["StartRestart"]["Integrity"]["KRRCint"].GetString();
      std::array<uint8_t, 32> k_rrc_int        = get_key_from_string(integ_key_string);
      log->debug_hex(k_rrc_int.data(), k_rrc_int.size(), "K_rrc_int");

      // get enc algo
      srslte::CIPHERING_ALGORITHM_ID_ENUM cipher_algo = {};
      std::string cipher_algo_string                  = as_sec["StartRestart"]["Ciphering"]["Algorithm"].GetString();
      if (cipher_algo_string == "eea0") {
        cipher_algo = srslte::CIPHERING_ALGORITHM_ID_EEA0;
      } else if (cipher_algo_string == "eea1") {
        cipher_algo = srslte::CIPHERING_ALGORITHM_ID_128_EEA1;
      } else if (cipher_algo_string == "eea2") {
        cipher_algo = srslte::CIPHERING_ALGORITHM_ID_128_EEA2;
      } else {
        log->error("Unsupported ciphering algorithm %s\n", cipher_algo_string.c_str());
      }

      // get cipher key
      std::string             cipher_key_string = as_sec["StartRestart"]["Ciphering"]["KRRCenc"].GetString();
      std::array<uint8_t, 32> k_rrc_enc         = get_key_from_string(cipher_key_string);
      log->debug_hex(k_rrc_enc.data(), k_rrc_enc.size(), "K_rrc_enc");

      // get UP enc key
      std::string             up_enc_key_string = as_sec["StartRestart"]["Ciphering"]["KUPenc"].GetString();
      std::array<uint8_t, 32> k_up_enc          = get_key_from_string(up_enc_key_string);
      log->debug_hex(k_up_enc.data(), k_up_enc.size(), "K_UP_enc");

      // parse ActTimeList
      ttcn3_helpers::pdcp_count_map_t bearers;
      if (as_sec["StartRestart"]["Ciphering"].HasMember("ActTimeList")) {
        const Value& act_time_list = as_sec["StartRestart"]["Ciphering"]["ActTimeList"];
        if (act_time_list.IsArray()) {
          for (Value::ConstValueIterator itr = act_time_list.Begin(); itr != act_time_list.End(); ++itr) {
            ttcn3_helpers::pdcp_count_t bearer = {};

            // obtain LCID and type
            if (itr->HasMember("RadioBearerId") && (*itr)["RadioBearerId"].HasMember("Srb")) {
              bearer.rb_is_srb = true;
              bearer.rb_id     = (*itr)["RadioBearerId"]["Srb"].GetInt();
            } else if (itr->HasMember("RadioBearerId") && (*itr)["RadioBearerId"].HasMember("Drb")) {
              bearer.rb_is_srb = false;
              bearer.rb_id     = (*itr)["RadioBearerId"]["Drb"].GetInt();
            }

            // obtain UL count
            if (itr->HasMember("UL")) {
              bearer.ul_value       = (*itr)["UL"]["SQN"]["Value"].GetInt();
              bearer.ul_value_valid = true;
            }

            // obtain DL count
            if (itr->HasMember("DL")) {
              bearer.dl_value       = (*itr)["DL"]["SQN"]["Value"].GetInt();
              bearer.dl_value_valid = true;
            }

            // append to list
            bearers.push_back(bearer);
          }
        }
      }

      // configure SS to use AS security
      syssim->set_as_security(ttcn3_helpers::get_timing_info(document),
                              cell_id.GetString(),
                              k_rrc_enc,
                              k_rrc_int,
                              k_up_enc,
                              cipher_algo,
                              integ_algo,
                              bearers);
    } else if (as_sec.HasMember("Release")) {
      // release all security configs
      syssim->release_as_security(ttcn3_helpers::get_timing_info(document), cell_id.GetString());
    }

    if (config_flag.GetBool() == true) {
      std::string resp = ttcn3_helpers::get_basic_sys_req_cnf(cell_id.GetString(), "AS_Security");
      log->info("Sending %s to tester (%zd B)\n", resp.c_str(), resp.length());
      send((const uint8_t*)resp.c_str(), resp.length());
    } else {
      log->info("Skipping response for AS_Security message.\n");
    }
  }

  std::array<uint8_t, 32> get_key_from_string(const std::string& str)
  {
    std::array<uint8_t, 32> key = {};
    if (str.size() == 128) {
      for (int i = 0; i < 16; i++) {
        std::string byte_string(str, i * 8, 8);
        key.at(i + 16) = std::stoul(byte_string, 0, 2);
      }
    }
    return key;
  }

  void handle_request_enquire_timing(Document& document)
  {
    const Value& a = document["Common"];

    assert(a.HasMember("CellId"));
    const Value& cell_id = a["CellId"];

    // check cnf flag
    assert(a.HasMember("ControlInfo"));
    const Value& b = a["ControlInfo"];
    assert(b.HasMember("CnfFlag"));

    // check request
    const Value& req = document["Request"];
    assert(req.HasMember("EnquireTiming"));

    std::string resp =
        ttcn3_helpers::get_sys_req_cnf_with_time(cell_id.GetString(), "EnquireTiming", syssim->get_tti());

    log->info("Sending %s to tester (%zd B)\n", resp.c_str(), resp.length());
    send((const uint8_t*)resp.c_str(), resp.length());
  }

  void handle_request_paging(Document& document, const uint8_t* payload, const uint16_t len)
  {
    const Value& a = document["Common"];

    assert(a.HasMember("CellId"));
    const Value& cell_id = a["CellId"];

    // check cnf flag
    assert(a.HasMember("ControlInfo"));
    const Value& b = a["ControlInfo"];
    assert(b.HasMember("CnfFlag"));

    // check request
    const Value& req = document["Request"];
    assert(req.HasMember("Paging"));

    // Pack payload into
    // Inform SYSSIM about paging

    uint16_t       consumed_bytes = 0;
    const uint8_t* payload_ptr    = payload;

    uint16_t tb_len = ((uint16_t)payload_ptr[0] << 8) | payload_ptr[1];
    payload_ptr += 2;

    unique_byte_buffer_t pch = pool_allocate_blocking;
    memcpy(pch->msg, payload_ptr, tb_len);
    payload_ptr += tb_len;
    pch->N_bytes = tb_len;

    // Push to main component
    log->info_hex(pch->msg, pch->N_bytes, "Received PCH DL-SCH\n");
    syssim->add_pch_pdu(std::move(pch));

    if (ttcn3_helpers::requires_confirm(document)) {
      std::string resp = ttcn3_helpers::get_sys_req_cnf_with_time(cell_id.GetString(), "Paging", syssim->get_tti());

      log->info("Sending %s to tester (%zd B)\n", resp.c_str(), resp.length());
      send((const uint8_t*)resp.c_str(), resp.length());
    } else {
      log->info("Skipping response for Paging message.\n");
    }
  }

  void handle_request_pdcp_handover_control(Document& document)
  {
    const Value& a = document["Common"];

    assert(a.HasMember("CellId"));
    const Value& cell_id = a["CellId"];

    // check cnf flag
    assert(a.HasMember("ControlInfo"));
    const Value& b = a["ControlInfo"];
    assert(b.HasMember("CnfFlag"));

    // check request
    const Value& req = document["Request"];
    assert(req.HasMember("PdcpHandoverControl"));

    const Value& ho_control = req["PdcpHandoverControl"];

    if (ho_control.HasMember("HandoverInit")) {
      const Value& ho_init = ho_control["HandoverInit"];
      assert(ho_init.HasMember("SourceCellId"));
      const Value& ho_source_cellid = ho_init["SourceCellId"];
    }

    // send confirm
    if (ttcn3_helpers::requires_confirm(document)) {
      std::string resp = ttcn3_helpers::get_basic_sys_req_cnf(cell_id.GetString(), "PdcpHandoverControl");

      log->info("Sending %s to tester (%zd B)\n", resp.c_str(), resp.length());
      send((const uint8_t*)resp.c_str(), resp.length());
    }
  }

  ss_sys_interface* syssim = nullptr;
  byte_buffer_pool* pool   = nullptr;
};

#endif // SRSUE_TTCN3_SYS_INTERFACE_H
