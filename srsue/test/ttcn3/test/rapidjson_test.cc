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

#include "ttcn3_helpers.h"
#include <assert.h>
#include <iostream>
#include <srsue/test/ttcn3/hdr/ttcn3_helpers.h>
#include <stdio.h>
#include <vector>

int SYSTEM_CTRL_CNF_test()
{
  char titan_result[] = "{\"Common\":{\"CellId\":\"eutra_Cell1\",\"RoutingInfo\":{\"None\":true},\"TimingInfo\":{"
                        "\"Now\":true},\"Result\":{\"Success\":true}},\"Confirm\":{\"Cell\":true}}";

  printf("%s\n", titan_result);

  printf("SYSTEM_CTRL_CNF_test()\n");

  const char cellid[] = "eutra_Cell1";

  Document resp;
  resp.SetObject();

  // Create members of common object

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
  common.AddMember("CellId", cellid, resp.GetAllocator());
  common.AddMember("RoutingInfo", routing_info, resp.GetAllocator());
  common.AddMember("TimingInfo", timing_info, resp.GetAllocator());
  common.AddMember("Result", result, resp.GetAllocator());

  // The confirm object
  Value confirm(kObjectType);
  confirm.AddMember("Cell", true, resp.GetAllocator());

  resp.AddMember("Common", common, resp.GetAllocator());
  resp.AddMember("Confirm", confirm, resp.GetAllocator());

  // printf("SYSTEM_CTRL_CNF\n%s\n", (char*)buffer.GetString());

  return 0;
}

void pretty_print(std::string json)
{
  Document document;
  if (document.Parse((char*)json.c_str()).HasParseError()) {
    fprintf(stderr, "Error parsing incoming data. Exiting\n");
    exit(-1);
  }
  assert(document.IsObject());

  // Pretty-print
  StringBuffer               buffer;
  PrettyWriter<StringBuffer> writer(buffer);
  document.Accept(writer);
  printf("%s\n", (char*)buffer.GetString());
}

// UDP v4 test
void IP_SOCK_CTRL_udp_v4_test()
{
  printf("IP_SOCK_CTRL_udp_v4_test()\n");
  char titan_result[] = "{\\\"CTRL\\\":{\\\"ConnectionId\\\":{\\\"Protocol\\\":\\\"udp\\\",\\\"Local\\\":{"
                        "\\\"IpAddr\\\":{\\\"V4\\\":{\\\"Addr\\\":\\\"127.0.0.1\\\"}},\\\"Port\\\":11},\\\"Remote\\\":{"
                        "\\\"Port\\\":22}},\\\"Ind\\\":{\\\"UDP\\\":{\\\"SocketCnf\\\":true}}}}\")";
  printf("%s\n", titan_result);

  string resp = ttcn3_helpers::get_ctrl_cnf("udp", "V4", "127.0.0.1");

  pretty_print(resp);
};

// UDP v4 test
void IP_SOCK_CTRL_icmp_v6_test()
{
  printf("IP_SOCK_CTRL_icmp_v6_test()\n");
  // char titan_result[] =
  // "{\\\"CTRL\\\":{\\\"ConnectionId\\\":{\\\"Protocol\\\":\\\"udp\\\",\\\"Local\\\":{\\\"IpAddr\\\":{\\\"V4\\\":{\\\"Addr\\\":\\\"127.0.0.1\\\"}},\\\"Port\\\":11},\\\"Remote\\\":{\\\"Port\\\":22}},\\\"Ind\\\":{\\\"UDP\\\":{\\\"SocketCnf\\\":true}}}}\")";
  // printf("%s\n", titan_result);

  //{ V6 := { Addr := "", ScopeId := omit } }

  string resp = ttcn3_helpers::get_ctrl_cnf("icmp6", "V6", "127.0.0.1");

  pretty_print(resp);
};

void PdcpCountGetReq_test()
{
  // Titan encoding test
  //"{\"Common\":{\"CellId\":\"eutra_Cell1\",\"RoutingInfo\":{\"None\":true},\"TimingInfo\":{\"Now\":true},\"Result\":{\"Success\":true}},\"Confirm\":{\"PdcpCount\":{\"Get\":[{\"RadioBearerId\":{\"Srb\":1},\"UL\":{\"Format\":\"PdcpCount_Srb\",\"Value\":\"00000000000000000000000000000000\"},\"DL\":{\"Format\":\"PdcpCount_Srb\",\"Value\":\"00000000000000000000000000000000\"}}]}}}")

  std::vector<ttcn3_helpers::pdcp_count_t> bearers;
  ttcn3_helpers::pdcp_count_t              srb1;
  srb1.rb_is_srb = true;
  srb1.rb_id     = 1;
  srb1.dl_value  = 0;
  srb1.ul_value  = 1;
  bearers.push_back(srb1);

  string resp = ttcn3_helpers::get_pdcp_count_response("cell1", bearers);
  pretty_print(resp);
}

void EnquireTiming_test()
{
  string resp = ttcn3_helpers::get_sys_req_cnf_with_time("cell1", "EnquireTiming", 7289);
  pretty_print(resp);
}

int main(int argc, char** argv)
{
  // SYSTEM_CTRL_CNF_test();
  // IP_SOCK_CTRL_udp_v4_test();
  // IP_SOCK_CTRL_icmp_v6_test();
  // PdcpCountGetReq_test();
  EnquireTiming_test();

  return 0;
}
