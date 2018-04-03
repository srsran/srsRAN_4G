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
#include <fstream>
#include <errno.h>
#include <signal.h>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include "srslte/common/bcd_helpers.h"
#include "srsepc/hdr/mme/mme.h"
#include "srsepc/hdr/hss/hss.h"
#include "srsepc/hdr/spgw/spgw.h"

using namespace std;
using namespace srsepc;
namespace bpo = boost::program_options;

bool running = true;

void 
sig_int_handler(int signo){
  running = false;
}

typedef struct {
  std::string   s1ap_level;
  int           s1ap_hex_limit;
  std::string   gtpc_level;
  int           gtpc_hex_limit;
  std::string   spgw_level;
  int           spgw_hex_limit;
  std::string   hss_level;
  int           hss_hex_limit;
  std::string   all_level;
  int           all_hex_limit;
  std::string   filename;
}log_args_t;


typedef struct{
  mme_args_t   mme_args;
  hss_args_t   hss_args;
  spgw_args_t  spgw_args;
  log_args_t   log_args;
}all_args_t;

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
  string mme_apn;
  string spgw_bind_addr;
  string sgi_if_addr;
  string dns_addr;
  string hss_db_file;
  string hss_auth_algo;
  string log_filename;

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
    ("mme.dns_addr",        bpo::value<string>(&dns_addr)->default_value("8.8.8.8"),"IP address of the DNS server for the UEs")
    ("mme.apn",             bpo::value<string>(&mme_apn)->default_value(""),                   "Set Access Point Name (APN) for data services")
    ("hss.db_file",         bpo::value<string>(&hss_db_file)->default_value("ue_db.csv"),".csv file that stores UE's keys")
    ("hss.auth_algo",       bpo::value<string>(&hss_auth_algo)->default_value("milenage"),"HSS uthentication algorithm.")
    ("spgw.gtpu_bind_addr", bpo::value<string>(&spgw_bind_addr)->default_value("127.0.0.1"),"IP address of SP-GW for the S1-U connection")
    ("spgw.sgi_if_addr",    bpo::value<string>(&sgi_if_addr)->default_value("176.16.0.1"),"IP address of TUN interface for the SGi connection")

    ("log.s1ap_level",     bpo::value<string>(&args->log_args.s1ap_level),   "MME S1AP log level")
    ("log.s1ap_hex_limit", bpo::value<int>(&args->log_args.s1ap_hex_limit),  "MME S1AP log hex dump limit")
    ("log.gtpc_level",     bpo::value<string>(&args->log_args.gtpc_level),   "MME GTPC log level")
    ("log.gtpc_hex_limit", bpo::value<int>(&args->log_args.gtpc_hex_limit),  "MME GTPC log hex dump limit")
    ("log.spgw_level",     bpo::value<string>(&args->log_args.spgw_level),   "SPGW log level")
    ("log.spgw_hex_limit", bpo::value<int>(&args->log_args.spgw_hex_limit),  "SPGW log hex dump limit")
    //("log.gtpu_level",    bpo::value<string>(&args->log.gtpu_level),  "GTPU log level")
    ("log.hss_level",     bpo::value<string>(&args->log_args.hss_level),   "HSS log level")
    ("log.hss_hex_limit", bpo::value<int>(&args->log_args.hss_hex_limit),  "HSS log hex dump limit")
    //("log.gtpu_hex_limit",bpo::value<int>(&args->log.gtpu_hex_limit), "GTPU log hex dump limit")

    ("log.all_level",     bpo::value<string>(&args->log_args.all_level)->default_value("info"),   "ALL log level")
    ("log.all_hex_limit", bpo::value<int>(&args->log_args.all_hex_limit)->default_value(32),  "ALL log hex dump limit")

    ("log.filename",      bpo::value<string>(&args->log_args.filename)->default_value("/tmp/epc.log"),"Log filename")
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

  //Parsing Config File
  if (!vm.count("config_file")) {
      cout << "Error: Configuration file not provided" << endl;
      cout << "Usage: " << argv[0] << " [OPTIONS] config_file" << endl << endl;
      exit(0);
  } else {
      cout << "Reading configuration file " << config_file << "..." << endl;
      ifstream conf(config_file.c_str(), ios::in);
      if(conf.fail()) {
        cout << "Failed to read configuration file " << config_file << " - exiting" << endl;
        exit(1);
      }
      bpo::store(bpo::parse_config_file(conf, common), vm);
      bpo::notify(vm);
  }



  //Concert hex strings
  {
    std::stringstream sstr;
    sstr << std::hex << vm["mme.mme_group"].as<std::string>();
    sstr >> args->mme_args.s1ap_args.mme_group;
  }
  {
    std::stringstream sstr;
    sstr << std::hex << vm["mme.mme_code"].as<std::string>();
    uint16_t tmp; // Need intermediate uint16_t as uint8_t is treated as char
    sstr >> tmp;
    args->mme_args.s1ap_args.mme_code = tmp;
  }
  {
    std::stringstream sstr;
    sstr << std::hex << vm["mme.tac"].as<std::string>();
    sstr >> args->mme_args.s1ap_args.tac;
  }
  // Convert MCC/MNC strings
  if(!srslte::string_to_mcc(mcc, &args->mme_args.s1ap_args.mcc)) {
    cout << "Error parsing mme.mcc:" << mcc << " - must be a 3-digit string." << endl;
  }
  if(!srslte::string_to_mnc(mnc, &args->mme_args.s1ap_args.mnc)) {
    cout << "Error parsing mme.mnc:" << mnc << " - must be a 2 or 3-digit string." << endl;
  }
 
  // Convert MCC/MNC strings
  if(!srslte::string_to_mcc(mcc, &args->hss_args.mcc)) {
    cout << "Error parsing mme.mcc:" << mcc << " - must be a 3-digit string." << endl;
  }
  if(!srslte::string_to_mnc(mnc, &args->hss_args.mnc)) {
    cout << "Error parsing mme.mnc:" << mnc << " - must be a 2 or 3-digit string." << endl;
  }

  args->mme_args.s1ap_args.mme_bind_addr = mme_bind_addr;
  args->mme_args.s1ap_args.mme_name = mme_name;
  args->mme_args.s1ap_args.dns_addr = dns_addr;
  args->mme_args.s1ap_args.mme_apn = mme_apn;
  args->spgw_args.gtpu_bind_addr = spgw_bind_addr;
  args->spgw_args.sgi_if_addr = sgi_if_addr;
  args->hss_args.db_file = hss_db_file;
  args->hss_args.auth_algo = hss_auth_algo;

  // Apply all_level to any unset layers
  if (vm.count("log.all_level")) {
    if(!vm.count("log.s1ap_level")) {
      args->log_args.s1ap_level = args->log_args.all_level;
    }
    if(!vm.count("log.gtpc_level")) {
      args->log_args.gtpc_level = args->log_args.all_level;
    }
    if(!vm.count("log.spgw_level")) {
      args->log_args.spgw_level = args->log_args.all_level;
    }
    if(!vm.count("log.hss_level")) {
      args->log_args.hss_level = args->log_args.all_level;
    }
  }

  // Apply all_hex_limit to any unset layers
  if (vm.count("log.all_hex_limit")) {
    if(!vm.count("log.s1ap_hex_limit")) {
      args->log_args.s1ap_hex_limit = args->log_args.all_hex_limit;
    }
    if(!vm.count("log.gtpc_hex_limit")) {
      args->log_args.gtpc_hex_limit = args->log_args.all_hex_limit;
    }
    if(!vm.count("log.spgw_hex_limit")) {
      args->log_args.spgw_hex_limit = args->log_args.all_hex_limit;
    }
    if(!vm.count("log.hss_hex_limit")) {
      args->log_args.hss_hex_limit = args->log_args.all_hex_limit;
    }
  }
  return;
}

srslte::LOG_LEVEL_ENUM
level(std::string l)
{
  boost::to_upper(l);
  if("NONE" == l){
    return srslte::LOG_LEVEL_NONE;
  }else if("ERROR" == l){
    return srslte::LOG_LEVEL_ERROR;
  }else if("WARNING" == l){
    return srslte::LOG_LEVEL_WARNING;
  }else if("INFO" == l){
    return srslte::LOG_LEVEL_INFO;
  }else if("DEBUG" == l){
    return srslte::LOG_LEVEL_DEBUG;
  }else{
    return srslte::LOG_LEVEL_NONE;
  }
}

int
main (int argc,char * argv[] )
{  
  cout << endl <<"---  Software Radio Systems EPC  ---" << endl << endl;
  signal(SIGINT, sig_int_handler);
  signal(SIGTERM, sig_int_handler);
  signal(SIGKILL, sig_int_handler);

  all_args_t args;
  parse_args(&args, argc, argv); 
 
  srslte::logger_stdout logger_stdout;
  srslte::logger_file   logger_file;
  srslte::logger        *logger;

 
  /*Init logger*/
  if (!args.log_args.filename.compare("stdout")) {
    logger = &logger_stdout;
  } else {
    logger_file.init(args.log_args.filename);
    logger_file.log("\n---  Software Radio Systems EPC log ---\n\n");
    logger = &logger_file;
  }

  srslte::log_filter s1ap_log;
  s1ap_log.init("S1AP",logger);
  s1ap_log.set_level(level(args.log_args.s1ap_level));
  s1ap_log.set_hex_limit(args.log_args.s1ap_hex_limit);

  srslte::log_filter mme_gtpc_log;
  mme_gtpc_log.init("GTPC",logger);
  mme_gtpc_log.set_level(level(args.log_args.gtpc_level));
  mme_gtpc_log.set_hex_limit(args.log_args.gtpc_hex_limit);

  srslte::log_filter hss_log;
  hss_log.init("HSS ",logger);
  hss_log.set_level(level(args.log_args.hss_level));
  hss_log.set_hex_limit(args.log_args.hss_hex_limit);

  srslte::log_filter spgw_log;
  spgw_log.init("SPGW",logger);
  spgw_log.set_level(level(args.log_args.spgw_level));
  spgw_log.set_hex_limit(args.log_args.spgw_hex_limit);


  hss *hss = hss::get_instance();
  if (hss->init(&args.hss_args,&hss_log)) {
    cout << "Error initializing HSS" << endl;
    exit(1);
  }

  mme *mme = mme::get_instance();
  if (mme->init(&args.mme_args, &s1ap_log, &mme_gtpc_log, hss)) {
    cout << "Error initializing MME" << endl;
    exit(1);
  }

  spgw *spgw = spgw::get_instance();
  if (spgw->init(&args.spgw_args,&spgw_log)) {
    cout << "Error initializing SP-GW" << endl;
    exit(1);
  } 

  mme->start(); 
  spgw->start();
  while(running) {
    sleep(1);
  }

  mme->stop();
  mme->cleanup();   
  spgw->stop();
  spgw->cleanup();
  hss->stop();
  hss->cleanup();

  cout << std::endl <<"---  exiting  ---" << endl;  
  return 0;
}
