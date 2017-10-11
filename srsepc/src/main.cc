/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2017 Software Radio Systems Limited
 *
 * \section LICENSE
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
#include <iostream>
#include <errno.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/sctp.h>
#include <boost/program_options.hpp>
#include "mme/mme.h"

using namespace std;
using namespace srsepc;
namespace bpo = boost::program_options;

bool running = true;

void 
sig_int_handler(int signo){
  running = false;
}


/*
void
parse_args(all_args_t *args, int argc, char* argv[]) {

  string mme_code;
  string mme_group;
  string tac;
  string mcc;
  string mnc;

  // Command line only options
  bpo::options_description general("General options");
  general.add_options()
      ("help,h", "Produce help message")
      ("version,v", "Print version information and exit")
      ;

  // Command line or config file options
  bpo::options_description common("Configuration options");
  common.add_options()

    ("mme.enb_id",        bpo::value<string>(&enb_id)->default_value("0x0"),                       "eNodeB ID")
    ("mme.name",          bpo::value<string>(&args->enb.s1ap.enb_name)->default_value("srsenb01"), "eNodeB Name")
    ("mme.cell_id",       bpo::value<string>(&cell_id)->default_value("0x0"),                      "Cell ID")
    ("mme.tac",           bpo::value<string>(&tac)->default_value("0x0"),                          "Tracking Area Code")
    ("mme.mcc",           bpo::value<string>(&mcc)->default_value("001"),                          "Mobile Country Code")
    ("mme.mnc",           bpo::value<string>(&mnc)->default_value("01"),                           "Mobile Network Code")
    ("mme.mme_bindx_addr",      bpo::value<string>(&args->enb.s1ap.mme_addr)->default_value("127.0.0.1"),"IP address of MME for S1 connnection")
    ;

  // Positional options - config file location
  bpo::options_description position("Positional options");
  position.add_options()
  ("config_file", bpo::value< string >(&config_file), "eNodeB configuration file")
  ;
  bpo::positional_options_description p;
  p.add("config_file", -1)

  return;
}
*/

int
main (int argc,char * argv[] )
{  
  cout << "---  Software Radio Systems EPC MME  ---" << endl << endl;
  
  signal(SIGINT,    sig_int_handler);

  //TODO these should be passed from config files
  all_args_t args; 
  args.s1ap_args.mme_code = 0x01;
  args.s1ap_args.mme_group = 0x0001;
  args.s1ap_args.tac = 0x0001;
  args.s1ap_args.mcc = 0x01;
  args.s1ap_args.mnc = 0x01;
  args.s1ap_args.mme_bindx_addr="127.0.0.0/24";

  struct sockaddr_in enb_addr;
  char readbuf[1000];
  struct sctp_sndrcvinfo sri;
  socklen_t fromlen;
  int rd_sz;
  int msg_flags=0;

  mme *mme = mme::get_instance();
  if (mme->init(&args)) {
    cout << "Error initializing MME" << endl;
    exit(1);
  }

  //Initalize S1-MME scoket
  int s1mme = mme->get_s1_mme();
  cout << "Socket: " << s1mme;
  while(running)
  {
    cout << "Waiting for SCTP Msg on: " << s1mme << endl;
    //cout << "Sri:" << sri <<endl;
    cout << "Flags:" << msg_flags <<endl;
    rd_sz = sctp_recvmsg(s1mme, (void*) readbuf, sizeof(readbuf),(struct sockaddr*) &enb_addr, &fromlen, &sri, &msg_flags);
    if (rd_sz == -1 && errno != EAGAIN){
      cout<< "Error reading from SCTP socket"<<endl;
      printf("Error: %s\n", strerror(errno));
      return -1;
    }
    else if (rd_sz == -1 && errno == EAGAIN){
      cout << "Timeout reached" << endl;
    }
    else{
      cout<< "Received SCTP msg." << endl;
      cout << "\tSize: " << rd_sz <<endl;
      cout << "\tMsg: " << readbuf << endl;
    }
  }
  mme->stop();
  mme->cleanup();  
  cout << "---  exiting  ---" << endl;  
  return 0;
}
