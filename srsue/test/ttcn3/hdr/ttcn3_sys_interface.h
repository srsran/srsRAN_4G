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

#ifndef SRSUE_TTCN3_SYS_INTERFACE_H
#define SRSUE_TTCN3_SYS_INTERFACE_H

#include "srslte/common/buffer_pool.h"
#include "ttcn3_helpers.h"
#include "ttcn3_interfaces.h"

using namespace srslte;

// The EUTRA.SYS interface
class ttcn3_sys_interface : public netsource_handler
{
public:
  ttcn3_sys_interface() : netsource_handler("TTCN3_SYS_IF"){};
  ~ttcn3_sys_interface(){};

  void init(syssim_interface* syssim_, srslte::log* log_, std::string net_ip_, uint32_t net_port_)
  {
    syssim      = syssim_;
    net_ip      = net_ip_;
    net_port    = net_port_;
    log         = log_;
    initialized = true;
    log->debug("Initialized.\n");
    pool = byte_buffer_pool::get_instance();
  }

private:
  void handle_request_cell_basic(Document& document, const uint8_t* payload, const uint16_t len)
  {
    if (document["Request"]["Cell"]["AddOrReconfigure"]["Basic"].HasMember("StaticCellInfo")) {
      // Extract EARFCN
      const Value& earfcn =
          document["Request"]["Cell"]["AddOrReconfigure"]["Basic"]["StaticCellInfo"]["Downlink"]["Earfcn"];
      assert(earfcn.IsInt());

      // Extract cell config
      const Value& common_config = document["Request"]["Cell"]["AddOrReconfigure"]["Basic"]["StaticCellInfo"]["Common"];
      const Value& dl_config = document["Request"]["Cell"]["AddOrReconfigure"]["Basic"]["StaticCellInfo"]["Downlink"];
      const Value& phy_dl_config = document["Request"]["Cell"]["AddOrReconfigure"]["Basic"]["PhysicalLayerConfigDL"];

      srslte_cell_t cell = {};
      cell.id            = common_config["PhysicalCellId"].GetInt();
      cell.cp = (strcmp(dl_config["CyclicPrefix"].GetString(), "normal") == 0) ? SRSLTE_CP_NORM : SRSLTE_CP_EXT;
      cell.nof_ports =
          (strcmp(phy_dl_config["AntennaGroup"]["AntennaInfoCommon"]["R8"]["antennaPortsCount"].GetString(), "an1") ==
           0)
              ? 1
              : 2;
      cell.nof_prb = (strcmp(dl_config["Bandwidth"].GetString(), "n25") == 0) ? 25 : 0;
      cell.phich_length =
          (strcmp(phy_dl_config["Phich"]["PhichConfig"]["R8"]["phich_Duration"].GetString(), "normal") == 0)
              ? SRSLTE_PHICH_NORM
              : SRSLTE_PHICH_EXT;
      cell.phich_resources =
          (strcmp(phy_dl_config["Phich"]["PhichConfig"]["R8"]["phich_Resource"].GetString(), "one") == 0)
              ? SRSLTE_PHICH_R_1
              : SRSLTE_PHICH_R_1_6;
      log->info("DL EARFCN is %d with n_prb=%d\n", earfcn.GetInt(), cell.nof_prb);

      const Value& ref_power =
          document["Request"]["Cell"]["AddOrReconfigure"]["Basic"]["InitialCellPower"]["MaxReferencePower"];
      assert(ref_power.IsInt());

      // That is the cellId or name that the testsuites uses to refer to a specific cell
      const Value& cell_name = document["Common"]["CellId"];
      assert(cell_name.IsString());

      // Now configure cell
      syssim->set_cell_config(cell_name.GetString(), earfcn.GetInt(), cell, ref_power.GetInt());

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
        log->info_hex(sib->msg, sib->N_bytes, "Received BCCH DL-SCH\n");
        syssim->add_bcch_pdu(std::move(sib));

        consumed_bytes = payload_ptr - payload;
      }

      // Create response for template car_CellConfig_CNF(CellId_Type p_CellId)
      std::string resp = ttcn3_helpers::get_basic_sys_req_cnf(cell_name.GetString(), "Cell");

      log->info("Sending %s to tester (%zd B)\n", resp.c_str(), resp.length());
      srslte_netsource_write(&net_source, (char*)resp.c_str(), resp.length());
    }
  }

  void handle_request_cell_active(Document& document, const uint8_t* payload, const uint16_t len)
  {
    // Create response for template car_CellConfig_CNF(CellId_Type p_CellId)
    std::string cell_id = document["Common"]["CellId"].GetString();

    std::string resp = ttcn3_helpers::get_basic_sys_req_cnf(cell_id, "Cell");

    log->info("Sending %s to tester (%zd B)\n", resp.c_str(), resp.length());
    srslte_netsource_write(&net_source, (char*)resp.c_str(), resp.length());
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
      srslte_netsource_write(&net_source, (char*)resp.c_str(), resp.length());
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
          uint32_t lcid = id["Srb"].GetInt();
          if (lcid > 0) {
            log->info("Configure SRB%d\n", lcid);
            pdcp_config_t pdcp_cfg = {.bearer_id    = static_cast<uint8_t>(lcid),
                                      .rb_type      = PDCP_RB_IS_SRB,
                                      .tx_direction = SECURITY_DIRECTION_DOWNLINK,
                                      .rx_direction = SECURITY_DIRECTION_UPLINK,
                                      .sn_len       = PDCP_SN_LEN_5};
            syssim->add_srb(lcid, pdcp_cfg);
          }
        } else if (config.HasMember("Release")) {
          log->info("Releasing SRB%d\n", id["Srb"].GetInt());
          uint32_t lcid = id["Srb"].GetInt();
          syssim->del_srb(lcid);
        } else {
          log->error("Unknown config.\n");
        }
      } else if (id.HasMember("Drb")) {
        log->info("Configure DRB%d\n", id["Drb"].GetInt());
      }

      // FIXME: actually do configuration
    }

    std::string resp = ttcn3_helpers::get_basic_sys_req_cnf(cell_id.GetString(), "RadioBearerList");

    log->info("Sending %s to tester (%zd B)\n", resp.c_str(), resp.length());
    srslte_netsource_write(&net_source, (char*)resp.c_str(), resp.length());
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
      syssim->set_cell_attenuation(id.GetString(), att_value);
    }

    std::string resp = ttcn3_helpers::get_basic_sys_req_cnf(cell_id.GetString(), "CellAttenuationList");

    log->info("Sending %s to tester (%zd B)\n", resp.c_str(), resp.length());
    srslte_netsource_write(&net_source, (char*)resp.c_str(), resp.length());
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

    // prepare response to SS
    std::vector<ttcn3_helpers::pdcp_count_t> bearers;
    ttcn3_helpers::pdcp_count_t              srb1;
    srb1.rb_is_srb = true;
    srb1.rb_id     = 1;
    srb1.dl_value  = 0;
    srb1.ul_value  = 1;
    bearers.push_back(srb1);

    std::string resp = ttcn3_helpers::get_pdcp_count_response(cell_id.GetString(), bearers);

    log->info("Sending %s to tester (%zd B)\n", resp.c_str(), resp.length());
    srslte_netsource_write(&net_source, (char*)resp.c_str(), resp.length());
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
      } else if (int_algo_string == "eea1") {
        cipher_algo = srslte::CIPHERING_ALGORITHM_ID_128_EEA1;
      } else if (int_algo_string == "eea2") {
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

      // get LCID
      uint32_t lcid = 0;
      if (as_sec["StartRestart"]["Ciphering"].HasMember("ActTimeList")) {
        const Value& act_time_list = as_sec["StartRestart"]["Ciphering"]["ActTimeList"];
        if (act_time_list.IsArray()) {
          for (Value::ConstValueIterator itr = act_time_list.Begin(); itr != act_time_list.End(); ++itr) {
            if (itr->HasMember("RadioBearerId") && (*itr)["RadioBearerId"].HasMember("Srb")) {
              lcid = (*itr)["RadioBearerId"]["Srb"].GetInt();
            }
          }
        }
      }

      // configure SS to use AS security
      syssim->set_as_security(lcid, k_rrc_enc, k_rrc_int, k_up_enc, cipher_algo, integ_algo);
    }

    if (config_flag.GetBool() == true) {
      std::string resp = ttcn3_helpers::get_basic_sys_req_cnf(cell_id.GetString(), "AS_Security");
      log->info("Sending %s to tester (%zd B)\n", resp.c_str(), resp.length());
      srslte_netsource_write(&net_source, (char*)resp.c_str(), resp.length());
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
    srslte_netsource_write(&net_source, (char*)resp.c_str(), resp.length());
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
      srslte_netsource_write(&net_source, (char*)resp.c_str(), resp.length());
    } else {
      log->info("Skipping response for Paging message.\n");
    }
  }

  void run_thread()
  {
    if (!initialized) {
      fprintf(stderr, "SYS interface not initialized. Exiting.\n");
      exit(-1);
    }

    // open TCP socket
    if (srslte_netsource_init(&net_source, net_ip.c_str(), net_port, SRSLTE_NETSOURCE_TCP)) {
      fprintf(stderr, "Error creating input TCP socket at port %d\n", net_port);
      exit(-1);
    }

    log->info("Listening on %s:%d for incoming connections ..\n", net_ip.c_str(), net_port);

    running = true;

    int n;
    while (run_enable) {
      log->debug("Reading from SYS port ..\n");
      n = srslte_netsource_read(&net_source, rx_buf->begin(), RX_BUF_SIZE);
      if (n > 0) {
        rx_buf->at(n) = '\0';

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
        if (document.Parse(json).HasParseError()) {
          log->error_hex((uint8*)json, json_len, "Error parsing incoming data.\n");
          break;
        }
        assert(document.IsObject());

        // Pretty-print
        StringBuffer               buffer;
        PrettyWriter<StringBuffer> writer(buffer);
        document.Accept(writer);
        log->info("Received %d bytes\n%s\n", json_len, (char*)buffer.GetString());

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
        } else {
          log->error("Received unknown request.\n");
        }
      } else {
        log->error("Error receiving from network\n");
      }
    }
    running = false;

    srslte_netsource_free(&net_source);
  }

  phy_interface_syssim* phy    = nullptr;
  syssim_interface*     syssim = nullptr;
  byte_buffer_pool*     pool   = nullptr;
};

#endif // SRSUE_TTCN3_SYS_INTERFACE_H