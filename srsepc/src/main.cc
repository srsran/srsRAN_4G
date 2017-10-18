/**
 *
 * \section COPYRIGHT
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
#include <boost/program_options.hpp>
#include "srslte/common/bcd_helpers.h"
#include "mme/mme.h"

using namespace std;
using namespace srsepc;
namespace bpo = boost::program_options;

bool running = true;

void 
sig_int_handler(int signo){
  running = false;
}

/**********************************************************************
 *  Program arguments processing
 ***********************************************************************/
string config_file;

void
parse_args(all_args_t *args, int argc, char* argv[]) {

  string mme_name;
  string mme_code;
  string mme_group;
  string tac;
  string mcc;
  string mnc;
  string mme_bind_addr;

  // Command line only options
  bpo::options_description general("General options");
  general.add_options()
      ("help,h", "Produce help message")
      ("version,v", "Print version information and exit")
      ;

  // Command line or config file options
  bpo::options_description common("Configuration options");
  common.add_options()

    ("mme.mme_code",        bpo::value<string>(&mme_code)->default_value("0x01"),                    "MME Code")
    ("mme.name",            bpo::value<string>(&mme_name)->default_value("srsmme01"), "MME Name")
    ("mme.mme_group",       bpo::value<string>(&mme_group)->default_value("0x01"),                     "Cell ID")
    ("mme.tac",             bpo::value<string>(&tac)->default_value("0x0"),                          "Tracking Area Code")
    ("mme.mcc",             bpo::value<string>(&mcc)->default_value("001"),                          "Mobile Country Code")
    ("mme.mnc",             bpo::value<string>(&mnc)->default_value("01"),                           "Mobile Network Code")
    ("mme.mme_bind_addr",   bpo::value<string>(&mme_bind_addr)->default_value("127.0.0.1"),"IP address of MME for S1 connnection")
    ;
  
  
  // Positional options - config file location
  bpo::options_description position("Positional options");
  position.add_options()
  ("config_file", bpo::value< string >(&config_file), "MME configuration file")
  ;
  bpo::positional_options_description p;
  p.add("config_file", -1);

  // these options are allowed on the command line
  bpo::options_description cmdline_options;
  cmdline_options.add(common).add(position).add(general);  

  // parse the command line and store result in vm
  bpo::variables_map vm;
  bpo::store(bpo::command_line_parser(argc, argv).options(cmdline_options).positional(p).run(), vm);
  bpo::notify(vm);

  // help option was given - print usage and exit
  if (vm.count("help")) {
      cout << "Usage: " << argv[0] << " [OPTIONS] config_file" << endl << endl;
      cout << common << endl << general << endl;
      exit(0);
  }
  //Concert hex strings
  {
    std::stringstream sstr;
    sstr << std::hex << vm["mme.mme_group"].as<std::string>();
    sstr >> args->s1ap_args.mme_group;
  }
  {
    std::stringstream sstr;
    sstr << std::hex << vm["mme.mme_code"].as<std::string>();
    uint16_t tmp; // Need intermediate uint16_t as uint8_t is treated as char
    sstr >> tmp;
    args->s1ap_args.mme_code = tmp;
  }
  {
    std::stringstream sstr;
    sstr << std::hex << vm["mme.tac"].as<std::string>();
    sstr >> args->s1ap_args.tac;
  }
  // Convert MCC/MNC strings
  if(!srslte::string_to_mcc(mcc, &args->s1ap_args.mcc)) {
    cout << "Error parsing enb.mcc:" << mcc << " - must be a 3-digit string." << endl;
  }
  if(!srslte::string_to_mnc(mnc, &args->s1ap_args.mnc)) {
    cout << "Error parsing enb.mnc:" << mnc << " - must be a 2 or 3-digit string." << endl;
  }
  
  args->s1ap_args.mme_bind_addr = mme_bind_addr;
  return;
}


int
main (int argc,char * argv[] )
{  
  cout << "---  Software Radio Systems EPC  ---" << endl << endl;
  signal(SIGINT, sig_int_handler);

  all_args_t args;
  parse_args(&args, argc, argv); 
  
  args.log_args.filename = std::string("/tmp/epc.log");

  mme *mme = mme::get_instance();
  if (mme->init(&args)) {
    cout << "Error initializing MME" << endl;
    exit(1);
  }
  mme->start();
  
  while(running) {
    sleep(0.5);
  }

  mme->stop();
  mme->cleanup();  

  cout << "---  exiting  ---" << endl;  
  return 0;
}
