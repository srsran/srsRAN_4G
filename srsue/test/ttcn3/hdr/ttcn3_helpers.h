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

/*! \brief This class provides helpers for encoding JSON-formated responses
 *         for TTCN3.
 *
 */

#ifndef SRSUE_TTCN3_HELPERS_H
#define SRSUE_TTCN3_HELPERS_H

#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON
#include <algorithm>
#include <assert.h>
#include <bitset>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
using namespace rapidjson;

class ttcn3_helpers
{
public:
  typedef struct {
    bool     now; ///< If set to false, the TTI field contains a valid TTI
    uint32_t tti;
  } timing_info_t;

  typedef struct {
    uint8_t  rb_id;
    bool     rb_is_srb;
    bool     ul_value_valid;
    uint16_t ul_value;
    bool     dl_value_valid;
    uint16_t dl_value;
  } pdcp_count_t;
  typedef std::vector<ttcn3_helpers::pdcp_count_t> pdcp_count_map_t;

  static std::string get_ctrl_cnf(const std::string protocol_, const std::string version_, const std::string addr_)
  {
    Document resp;
    resp.SetObject();

    // Create members of common object
    Value ctrl(kObjectType);

    // The connection ID object
    Value conn_id(kObjectType);

    // Protocol
    Value protocol(protocol_.c_str(), resp.GetAllocator());
    conn_id.AddMember("Protocol", protocol, resp.GetAllocator());

    // Version
    Value ipAddr(kObjectType);
    Value version(version_.c_str(), resp.GetAllocator());
    Value addr(addr_.c_str(), resp.GetAllocator());

    if (version_ == "V4") {
      Value v4(kObjectType);
      v4.AddMember("Addr", addr, resp.GetAllocator());
      ipAddr.AddMember("V4", v4, resp.GetAllocator());
    } else if (version_ == "V6") {
      Value v6(kObjectType);
      v6.AddMember("Addr", addr, resp.GetAllocator());
      ipAddr.AddMember("V6", v6, resp.GetAllocator());
    } else {
      fprintf(stderr, "Unsupported protocol version: %s.\n", version_.c_str());
      return std::string("");
    }

    // Local
    Value local(kObjectType);
    local.AddMember("IpAddr", ipAddr, resp.GetAllocator());

    if (protocol_ == "udp") {
      local.AddMember("Port", 33, resp.GetAllocator());
    }

    // Remote
    Value rem(kObjectType);

    // Add members to Connection ID
    conn_id.AddMember("Local", local, resp.GetAllocator());
    conn_id.AddMember("Remote", rem, resp.GetAllocator());

    // The Indication object
    Value ind(kObjectType);
    Value ind_protocol_val(kObjectType);
    ind_protocol_val.AddMember("SocketCnf", true, resp.GetAllocator());
    if (protocol_ == "udp") {
      ind.AddMember("UDP", ind_protocol_val, resp.GetAllocator());
    } else if (protocol_ == "icmpv6") {
      ind.AddMember("ICMP", ind_protocol_val, resp.GetAllocator());
    } else {
      fprintf(stderr, "Unsupported protocol %s.\n", protocol_.c_str());
      return std::string("");
    }

    ctrl.AddMember("ConnectionId", conn_id, resp.GetAllocator());
    ctrl.AddMember("Ind", ind, resp.GetAllocator());

    resp.AddMember("CTRL", ctrl, resp.GetAllocator());

    // JSON-ize
    StringBuffer         buffer;
    Writer<StringBuffer> writer(buffer);
    resp.Accept(writer);

    // Return as std::string
    return std::string(buffer.GetString());
  }

  static std::string get_drbmux_common_ind_cnf()
  {
    Document resp;
    resp.SetObject();

    resp.AddMember("Confirm", true, resp.GetAllocator());

    // JSON-ize
    StringBuffer         buffer;
    Writer<StringBuffer> writer(buffer);
    resp.Accept(writer);

    // Return as std::string
    return std::string(buffer.GetString());
  }

  static std::string get_pdcp_count_response(const std::string                               cell_,
                                             const std::vector<ttcn3_helpers::pdcp_count_t>& bearers)
  {
    Document resp;
    resp.SetObject();

    // Create members of common object

    // Cell
    Value cell(cell_.c_str(), resp.GetAllocator());

    // RoutingInfo
    Value routing_info(kObjectType);
    routing_info.AddMember("None", true, resp.GetAllocator());

    // TimingInfo
    Value timing_info(kObjectType);
    timing_info.AddMember("Now", true, resp.GetAllocator());

    // Result
    Value result(kObjectType);
    result.AddMember("Success", true, resp.GetAllocator());

    // Now, create the common object itself and add members
    Value common(kObjectType);
    common.AddMember("CellId", cell, resp.GetAllocator());
    common.AddMember("RoutingInfo", routing_info, resp.GetAllocator());
    common.AddMember("TimingInfo", timing_info, resp.GetAllocator());
    common.AddMember("Result", result, resp.GetAllocator());

    // Add all bearers
    rapidjson::Value                                         bearer_array(rapidjson::kArrayType);
    std::vector<ttcn3_helpers::pdcp_count_t>::const_iterator it;
    for (it = bearers.begin(); it != bearers.end(); ++it) {
      // SRB0
      Value radio_bearer_id(kObjectType);
      radio_bearer_id.AddMember(it->rb_is_srb ? "Srb" : "Drb", it->rb_id, resp.GetAllocator());

      Value ul(kObjectType);
      Value rb_format_ul(it->rb_is_srb ? "PdcpCount_Srb" : "PdcpCount_DrbLongSQN", resp.GetAllocator());
      ul.AddMember("Format", rb_format_ul, resp.GetAllocator());

      // convert int to sstream, then to RapidJSON value and add as member
      stringstream value_ss;
      value_ss << std::bitset<32>(it->ul_value);
      Value ul_value(value_ss.str().c_str(), resp.GetAllocator());
      ul.AddMember("Value", ul_value, resp.GetAllocator());

      Value dl(kObjectType);
      Value rb_format_dl(it->rb_is_srb ? "PdcpCount_Srb" : "PdcpCount_DrbLongSQN", resp.GetAllocator());
      dl.AddMember("Format", rb_format_dl, resp.GetAllocator());

      // do the same conversion for the DL value
      value_ss.str("");
      value_ss.clear();
      value_ss << std::bitset<32>(it->dl_value);
      Value dl_value(value_ss.str().c_str(), resp.GetAllocator());
      dl.AddMember("Value", dl_value, resp.GetAllocator());

      // The Get object combines the information of all requested bearer
      Value get_element(kObjectType);
      get_element.AddMember("RadioBearerId", radio_bearer_id, resp.GetAllocator());
      get_element.AddMember("UL", ul, resp.GetAllocator());
      get_element.AddMember("DL", dl, resp.GetAllocator());

      bearer_array.PushBack(get_element, resp.GetAllocator());
    }

    // The confirm object
    Value confirm_key(kObjectType);

    // Add array
    confirm_key.AddMember("Get", bearer_array, resp.GetAllocator());

    Value confirm(kObjectType);
    confirm.AddMember("PdcpCount", confirm_key, resp.GetAllocator());

    resp.AddMember("Common", common, resp.GetAllocator());
    resp.AddMember("Confirm", confirm, resp.GetAllocator());

    // JSON-ize
    StringBuffer         buffer;
    Writer<StringBuffer> writer(buffer);
    resp.Accept(writer);

    // Return as std::string
    return std::string(buffer.GetString());
  }

  static std::string get_basic_sys_req_cnf(const std::string cell_, const std::string confirm_key_)
  {
    Document resp;
    resp.SetObject();

    // Create members of common object

    // Cell
    Value cell(cell_.c_str(), resp.GetAllocator());

    // RoutingInfo
    Value routing_info(kObjectType);
    routing_info.AddMember("None", true, resp.GetAllocator());

    // TimingInfo
    Value timing_info(kObjectType);
    timing_info.AddMember("Now", true, resp.GetAllocator());

    // Result
    Value result(kObjectType);
    result.AddMember("Success", true, resp.GetAllocator());

    // Now, create the common object itself and add members
    Value common(kObjectType);
    common.AddMember("CellId", cell, resp.GetAllocator());
    common.AddMember("RoutingInfo", routing_info, resp.GetAllocator());
    common.AddMember("TimingInfo", timing_info, resp.GetAllocator());
    common.AddMember("Result", result, resp.GetAllocator());

    // The confirm object
    Value confirm_key(confirm_key_.c_str(), resp.GetAllocator());
    Value confirm_val(true);
    Value confirm(kObjectType);
    confirm.AddMember(confirm_key, confirm_val, resp.GetAllocator());

    resp.AddMember("Common", common, resp.GetAllocator());
    resp.AddMember("Confirm", confirm, resp.GetAllocator());

    // JSON-ize
    StringBuffer         buffer;
    Writer<StringBuffer> writer(buffer);
    resp.Accept(writer);

    // Return as std::string
    return std::string(buffer.GetString());
  }

  static std::string
  get_sys_req_cnf_with_time(const std::string cell_, const std::string confirm_key_, const uint32_t tti)
  {
    Document resp;
    resp.SetObject();

    // Create members of common object

    // Cell
    Value cell(cell_.c_str(), resp.GetAllocator());

    // RoutingInfo
    Value routing_info(kObjectType);
    routing_info.AddMember("None", true, resp.GetAllocator());

    // TimingInfo

    // SFN
    uint32_t sfn = tti / 10;
    Value    sfn_key(kObjectType);
    sfn_key.AddMember("Number", sfn, resp.GetAllocator());

    // Actual subframe index
    uint32_t sf_idx = tti % 10;
    Value    sf_idx_key(kObjectType);
    sf_idx_key.AddMember("Number", sf_idx, resp.GetAllocator());

    // Put it all together
    Value subframe_key(kObjectType);
    subframe_key.AddMember("SFN", sfn_key, resp.GetAllocator());
    subframe_key.AddMember("Subframe", sf_idx_key, resp.GetAllocator());

    Value timing_info(kObjectType);
    timing_info.AddMember("SubFrame", subframe_key, resp.GetAllocator());

    // Result
    Value result(kObjectType);
    result.AddMember("Success", true, resp.GetAllocator());

    // Now, create the common object itself and add members
    Value common(kObjectType);
    common.AddMember("CellId", cell, resp.GetAllocator());
    common.AddMember("RoutingInfo", routing_info, resp.GetAllocator());
    common.AddMember("TimingInfo", timing_info, resp.GetAllocator());
    common.AddMember("Result", result, resp.GetAllocator());

    // The confirm object
    Value confirm_key(confirm_key_.c_str(), resp.GetAllocator());
    Value confirm_val(true);
    Value confirm(kObjectType);
    confirm.AddMember(confirm_key, confirm_val, resp.GetAllocator());

    resp.AddMember("Common", common, resp.GetAllocator());
    resp.AddMember("Confirm", confirm, resp.GetAllocator());

    // JSON-ize
    StringBuffer         buffer;
    Writer<StringBuffer> writer(buffer);
    resp.Accept(writer);

    // Return as std::string
    return std::string(buffer.GetString());
  }

  static bool requires_confirm(Document& document)
  {
    const Value& a = document["Common"];

    // check cnf flag
    assert(a.HasMember("ControlInfo"));
    const Value& b = a["ControlInfo"];
    assert(b.HasMember("CnfFlag"));

    const Value& config_flag = b["CnfFlag"];
    assert(config_flag.IsBool());

    return config_flag.GetBool();
  }

  static timing_info_t get_timing_info(Document& document)
  {
    timing_info_t timing = {};

    // check for Now flag
    if (document.HasMember("Common") && document["Common"].HasMember("TimingInfo") &&
        document["Common"]["TimingInfo"].HasMember("Now")) {
      timing.now = true;
    }

    if (document.HasMember("Common") && document["Common"].HasMember("TimingInfo") &&
        document["Common"]["TimingInfo"].HasMember("SubFrame") &&
        document["Common"]["TimingInfo"]["SubFrame"].HasMember("SFN") &&
        document["Common"]["TimingInfo"]["SubFrame"]["SFN"].HasMember("Number")) {

      timing.tti = document["Common"]["TimingInfo"]["SubFrame"]["SFN"]["Number"].GetInt() * 10;

      // check SF index only
      if (document["Common"]["TimingInfo"]["SubFrame"].HasMember("Subframe") &&
          document["Common"]["TimingInfo"]["SubFrame"]["Subframe"].HasMember("Number")) {
        timing.tti += document["Common"]["TimingInfo"]["SubFrame"]["Subframe"]["Number"].GetInt();
      }
    }
    return timing;
  }

  static bool get_follow_on_flag(Document& document)
  {
    const Value& a = document["Common"];

    // check cnf flag
    assert(a.HasMember("ControlInfo"));
    const Value& b = a["ControlInfo"];
    assert(b.HasMember("FollowOnFlag"));

    const Value& config_flag = b["FollowOnFlag"];
    assert(config_flag.IsBool());

    return config_flag.GetBool();
  }
};

#endif // SRSUE_TTCN3_HELPERS_H
