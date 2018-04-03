#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <linux/ip.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <assert.h>

#include "srslte/phy/utils/debug.h"
#include "srsue/hdr/mac/mac.h"
#include "srsue/hdr/phy/phy.h"
#include "srslte/common/threads.h"
#include "srslte/common/common.h"
#include "srslte/common/buffer_pool.h"
#include "srslte/common/logger_file.h"
#include "srslte/common/log_filter.h"
#include "srslte/upper/rlc.h"
#include "srsue/hdr/upper/rrc.h"
#include "srslte/radio/radio_multi.h"

#define START_TUNTAP
#define USE_RADIO
#define PRINT_GW 0

/**********************************************************************
 *  Program arguments processing
 ***********************************************************************/

#define LCID 3

typedef struct {
  float rx_freq; 
  float tx_freq; 
  float rx_gain;
  float tx_gain;
  int time_adv;
  std::string ip_address;
}prog_args_t;

uint32_t srsapps_verbose = 1; 

prog_args_t prog_args; 

void args_default(prog_args_t *args) {
  args->tx_freq = 2.505e9;
  args->rx_freq = 2.625e9;
  args->rx_gain = 50.0; 
  args->tx_gain = 70.0; 
  args->time_adv = -1; // calibrated for b210
  args->ip_address = "192.168.3.2";
}

void usage(prog_args_t *args, char *prog) {
  printf("Usage: %s [gGIrfFtv]\n", prog);
  printf("\t-f RX frequency [Default %.1f MHz]\n", args->rx_freq/1e6);
  printf("\t-F TX frequency [Default %.1f MHz]\n", args->tx_freq/1e6);
  printf("\t-g RX gain [Default %.1f]\n", args->rx_gain);
  printf("\t-G TX gain [Default %.1f]\n", args->tx_gain);
  printf("\t-I IP address [Default %s]\n", args->ip_address.c_str());
  printf("\t-t time advance (in samples) [Default %d]\n", args->time_adv);
  printf("\t-v [increase verbosity, default none]\n");
}

void parse_args(prog_args_t *args, int argc, char **argv) {
  int opt;
  args_default(args);
  while ((opt = getopt(argc, argv, "gGfFItv")) != -1) {
    switch (opt) {
    case 'g':
      args->rx_gain = atof(argv[optind]);
      break;
    case 'G':
      args->tx_gain = atof(argv[optind]);
      break;
    case 'f':
      args->rx_freq = atof(argv[optind]);
      break;
    case 'F':
      args->tx_freq = atof(argv[optind]);
      break;
    case 'I':
      args->ip_address = argv[optind];
      break;
    case 't':
      args->time_adv = atoi(argv[optind]);
      break;
    case 'v':
      srsapps_verbose++;
      break;
    default:
      usage(args, argv[0]);
      exit(-1);
    }
  }
  if (args->rx_freq < 0 || args->tx_freq < 0) {
    usage(args, argv[0]);
    exit(-1);
  }
}
                            
int setup_if_addr(char *ip_addr);
  
// Define dummy RLC always transmitts
class tester : public srsue::pdcp_interface_rlc, 
               public srsue::rrc_interface_rlc,
               public srsue::rrc_interface_phy,
               public srsue::rrc_interface_mac,
               public srsue::ue_interface,
               public thread
{
public:
  
  tester() {
    state = srsue::RRC_STATE_SIB1_SEARCH;
    read_enable = true; 
  }
  
  void init(srsue::phy *phy_, srsue::mac *mac_, srslte::rlc *rlc_, srslte::log *log_h_, std::string ip_address) {
    log_h = log_h_; 
    rlc   = rlc_; 
    mac   = mac_; 
    phy   = phy_; 
    
#ifdef START_TUNTAP
    if (init_tuntap((char*) ip_address.c_str())) {
      log_h->error("Initiating IP address\n");
    }
#endif

    pool = srslte::byte_buffer_pool::get_instance();
    
    // Start reader thread
    running=true; 
    start();

  }
  

  void sib_search()
  {
    bool      searching = true;
    uint32_t  tti ;
    uint32_t  si_win_start, si_win_len;
    uint16_t  period;
    uint32_t  nof_sib1_trials = 0; 
    const int SIB1_SEARCH_TIMEOUT = 30; 

    while(searching)
    {
      switch(state)
      {
      case srsue::RRC_STATE_SIB1_SEARCH:
        // Instruct MAC to look for SIB1
        while(!phy->status_is_sync()){
          usleep(50000);
        }
        usleep(10000); 
        tti          = mac->get_current_tti();
        si_win_start = sib_start_tti(tti, 2, 5);
        mac->bcch_start_rx(si_win_start, 1);
        log_h->info("Instructed MAC to search for SIB1, win_start=%d, win_len=%d\n",
                      si_win_start, 1);
        nof_sib1_trials++;
        if (nof_sib1_trials >= SIB1_SEARCH_TIMEOUT) {
          log_h->info("Timeout while searching for SIB1. Resynchronizing SFN...\n");
          log_h->console("Timeout while searching for SIB1. Resynchronizing SFN...\n");
          phy->resync_sfn();
          nof_sib1_trials = 0; 
        }
        break;
      case srsue::RRC_STATE_SIB2_SEARCH:
        // Instruct MAC to look for SIB2
        usleep(10000);
        tti          = mac->get_current_tti();
        period       = liblte_rrc_si_periodicity_num[sib1.sched_info[0].si_periodicity];
        si_win_start = sib_start_tti(tti, period, 0);
        si_win_len   = liblte_rrc_si_window_length_num[sib1.si_window_length];

        mac->bcch_start_rx(si_win_start, si_win_len);
        log_h->info("Instructed MAC to search for SIB2, win_start=%d, win_len=%d\n",
                      si_win_start, si_win_len);

        break;
      default:
        searching = false;
        break;
      }
      usleep(100000);
    }
  }
  
  bool is_sib_received() {
    return state == srsue::RRC_STATE_WAIT_FOR_CON_SETUP; 
  }


  void release_pucch_srs() {}
  void ra_problem() {}
  void write_pdu_bcch_bch(srslte::byte_buffer_t *pdu) {}
  void write_pdu_bcch_dlsch(srslte::byte_buffer_t *pdu) 
  {
    log_h->info_hex(pdu->msg, pdu->N_bytes, "BCCH DLSCH message received.");
    log_h->info("BCCH DLSCH message Stack latency: %ld us\n", pdu->get_latency_us());
    LIBLTE_RRC_BCCH_DLSCH_MSG_STRUCT dlsch_msg;
    srslte_bit_unpack_vector(pdu->msg, bit_buf.msg, pdu->N_bytes*8);
    bit_buf.N_bits = pdu->N_bytes*8;
    pool->deallocate(pdu);
    liblte_rrc_unpack_bcch_dlsch_msg((LIBLTE_BIT_MSG_STRUCT*)&bit_buf, &dlsch_msg);

    if (dlsch_msg.N_sibs > 0) {
      if (LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1 == dlsch_msg.sibs[0].sib_type && srsue::RRC_STATE_SIB1_SEARCH == state) {
        // Handle SIB1
        memcpy(&sib1, &dlsch_msg.sibs[0].sib.sib1, sizeof(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1_STRUCT));
        log_h->info("SIB1 received, CellID=%d, si_window=%d, sib2_period=%d\n",
                      sib1.cell_id&0xfff,
                      liblte_rrc_si_window_length_num[sib1.si_window_length],
                      liblte_rrc_si_periodicity_num[sib1.sched_info[0].si_periodicity]);
        std::stringstream ss;
        for(uint32_t i=0;i<sib1.N_plmn_ids;i++){
          ss << " PLMN Id: MCC " << sib1.plmn_id[i].id.mcc << " MNC " << sib1.plmn_id[i].id.mnc;
        }
        log_h->console("SIB1 received, CellID=%d, %s\n",
                        sib1.cell_id&0xfff,
                        ss.str().c_str());
        
        state = srsue::RRC_STATE_SIB2_SEARCH;
        mac->bcch_stop_rx();
        //TODO: Use all SIB1 info

      } else if (LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2 == dlsch_msg.sibs[0].sib_type && srsue::RRC_STATE_SIB2_SEARCH == state) {
        // Handle SIB2
        memcpy(&sib2, &dlsch_msg.sibs[0].sib.sib2, sizeof(LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2_STRUCT));
        log_h->console("SIB2 received\n");
        log_h->info("SIB2 received\n");
        state = srsue::RRC_STATE_WAIT_FOR_CON_SETUP;
        mac->bcch_stop_rx();
        apply_sib2_configs();
        
        srslte::byte_buffer_t *sdu = pool_allocate; 
        assert(sdu); 
        
        // Send Msg3 
        sdu->N_bytes = 10; 
        for (uint32_t i=0;i<sdu->N_bytes;i++) {
          sdu->msg[i] = i+1; 
        }
        uint64_t uecri = 0; 
        uint8_t *ue_cri_ptr = (uint8_t*) &uecri;
        uint32_t nbytes = 6;
        for (uint32_t i=0;i<nbytes;i++) {
          ue_cri_ptr[nbytes-i-1] = sdu->msg[i];
        }
        log_h->info("Setting UE contention resolution ID: %d\n", uecri);        
        mac->set_contention_id(uecri);
        
        rlc->write_sdu(0, sdu);

      }
    }
  }
  void write_pdu_pcch(srslte::byte_buffer_t *sdu) {}
  void max_retx_attempted(){}
  std::string get_rb_name(uint32_t lcid) { return std::string("rb"); }
  void in_sync() {};
  void out_of_sync() {};

  void write_pdu(uint32_t lcid, srslte::byte_buffer_t *sdu)
  {
    uint32_t n=0;
    switch(lcid) {
      case LCID:
        n = write(tun_fd, sdu->msg, sdu->N_bytes);
        if (n != sdu->N_bytes) {
          log_h->error("TUN/TAP write failure n=%d, nof_bytes=%d\n", n, sdu->N_bytes);
          return; 
        }
        log_h->debug_hex(sdu->msg, sdu->N_bytes, 
                        "Wrote %d bytes to TUN/TAP\n", 
                        sdu->N_bytes);      
        pool->deallocate(sdu);
      break;
      case 0:
        log_h->info("Received ConnectionSetupComplete\n");
        
        // Setup a single UM bearer 
        LIBLTE_RRC_RLC_CONFIG_STRUCT cfg; 
        bzero(&cfg, sizeof(LIBLTE_RRC_RLC_CONFIG_STRUCT));
        cfg.rlc_mode = LIBLTE_RRC_RLC_MODE_UM_BI;
        cfg.dl_um_bi_rlc.t_reordering = LIBLTE_RRC_T_REORDERING_MS100; 
        cfg.dl_um_bi_rlc.sn_field_len = LIBLTE_RRC_SN_FIELD_LENGTH_SIZE10;   
        cfg.ul_um_bi_rlc.sn_field_len = LIBLTE_RRC_SN_FIELD_LENGTH_SIZE10;   
        rlc->add_bearer(LCID, &cfg);
        
        mac->setup_lcid(LCID, 0, 1, -1, 100000);

        LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT dedicated; 
        bzero(&dedicated, sizeof(LIBLTE_RRC_PHYSICAL_CONFIG_DEDICATED_STRUCT));
        dedicated.pusch_cnfg_ded.beta_offset_ack_idx = 5;
        dedicated.pusch_cnfg_ded.beta_offset_ri_idx  = 12;
        dedicated.pusch_cnfg_ded.beta_offset_cqi_idx = 15;
        dedicated.pusch_cnfg_ded_present = true; 
        dedicated.sched_request_cnfg.dsr_trans_max = LIBLTE_RRC_DSR_TRANS_MAX_N4; 
        dedicated.sched_request_cnfg.sr_pucch_resource_idx = 0;
        dedicated.sched_request_cnfg.sr_cnfg_idx = 35; 
        dedicated.sched_request_cnfg.setup_present = true; 
        dedicated.sched_request_cnfg_present = true; 
        phy->set_config_dedicated(&dedicated);
        phy->configure_ul_params();
        
        srsue::mac_interface_rrc::mac_cfg_t mac_cfg; 
        mac->get_config(&mac_cfg);
        memcpy(&mac_cfg.sr, &dedicated.sched_request_cnfg, sizeof(LIBLTE_RRC_SCHEDULING_REQUEST_CONFIG_STRUCT));
        mac_cfg.main.ulsch_cnfg.periodic_bsr_timer = LIBLTE_RRC_PERIODIC_BSR_TIMER_SF40;
        mac->set_config(&mac_cfg);
    
      break;
      default:
        log_h->error("Received message for lcid=%d\n", lcid);
      break;
    }
  }
  
private:
  int tun_fd;
  bool running; 
  srslte::log *log_h;
  srslte::byte_buffer_pool *pool;
  srslte::rlc *rlc; 
  srsue::mac *mac; 
  srsue::phy *phy;  
  srslte::bit_buffer_t bit_buf;
  srsue::rrc_state_t state; 
  LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_1_STRUCT sib1;
  LIBLTE_RRC_SYS_INFO_BLOCK_TYPE_2_STRUCT sib2;
  bool read_enable;

  
  // Determine SI messages scheduling as in 36.331 5.2.3 Acquisition of an SI message
  uint32_t sib_start_tti(uint32_t tti, uint32_t period, uint32_t x) {
    return (period*10*(1+tti/(period*10))+x)%10240; // the 1 means next opportunity
  }
    
  int init_tuntap(char *ip_address) {
    read_enable = true; 
    tun_fd = setup_if_addr(ip_address);
    if (tun_fd<0) {
      fprintf(stderr, "Error setting up IP %s\n", ip_address);
      return -1;
    }
    
    printf("Created tun/tap interface at IP %s\n", ip_address);
    return 0; 
  }
  
  void run_thread() {
    struct iphdr   *ip_pkt;
    uint32_t        idx = 0;
    int32_t         N_bytes;
    srslte::byte_buffer_t  *pdu = pool_allocate;

    log_h->info("TUN/TAP reader thread running\n");

    while(running) {
      N_bytes = read(tun_fd, &pdu->msg[idx], SRSLTE_MAX_BUFFER_SIZE_BYTES-SRSLTE_BUFFER_HEADER_OFFSET - idx);      
      if(N_bytes > 0 && read_enable)
      {
        pdu->N_bytes = idx + N_bytes;
        ip_pkt       = (struct iphdr*)pdu->msg;

        log_h->debug_hex(pdu->msg, pdu->N_bytes, 
                          "Read %d bytes from TUN/TAP\n", 
                          N_bytes);
                  
        // Check if entire packet was received
        if(ntohs(ip_pkt->tot_len) == pdu->N_bytes)
        {
          log_h->info_hex(pdu->msg, pdu->N_bytes, "UL PDU");

          // Send PDU directly to PDCP
          pdu->set_timestamp();
          rlc->write_sdu(LCID, pdu);
          
          pdu = pool_allocate;
          idx = 0;
        } else{
          idx += N_bytes;
        }
      }else{
        log_h->error("Failed to read from TUN interface - gw receive thread exiting.\n");
        break;
      }
    }
  }
  
    
  void apply_sib2_configs()
  {
    
    // Apply RACH timeAlginmentTimer configuration 
    srsue::mac_interface_rrc::mac_cfg_t cfg; 
    mac->get_config(&cfg);
    cfg.main.time_alignment_timer = sib2.time_alignment_timer; 
    memcpy(&cfg.rach, &sib2.rr_config_common_sib.rach_cnfg, sizeof(LIBLTE_RRC_RACH_CONFIG_COMMON_STRUCT)); 
    cfg.prach_config_index = sib2.rr_config_common_sib.prach_cnfg.root_sequence_index; 
    mac->set_config(&cfg);
    
    log_h->info("Set RACH ConfigCommon: NofPreambles=%d, ResponseWindow=%d, ContentionResolutionTimer=%d ms\n",
          liblte_rrc_number_of_ra_preambles_num[sib2.rr_config_common_sib.rach_cnfg.num_ra_preambles],
          liblte_rrc_ra_response_window_size_num[sib2.rr_config_common_sib.rach_cnfg.ra_resp_win_size],
          liblte_rrc_mac_contention_resolution_timer_num[sib2.rr_config_common_sib.rach_cnfg.mac_con_res_timer]);

    // Apply PHY RR Config Common
    srsue::phy_interface_rrc::phy_cfg_common_t common; 
    memcpy(&common.pdsch_cnfg,  &sib2.rr_config_common_sib.pdsch_cnfg,  sizeof(LIBLTE_RRC_PDSCH_CONFIG_COMMON_STRUCT));
    memcpy(&common.pusch_cnfg,  &sib2.rr_config_common_sib.pusch_cnfg,  sizeof(LIBLTE_RRC_PUSCH_CONFIG_COMMON_STRUCT));
    memcpy(&common.pucch_cnfg,  &sib2.rr_config_common_sib.pucch_cnfg,  sizeof(LIBLTE_RRC_PUCCH_CONFIG_COMMON_STRUCT));
    memcpy(&common.ul_pwr_ctrl, &sib2.rr_config_common_sib.ul_pwr_ctrl, sizeof(LIBLTE_RRC_UL_POWER_CONTROL_COMMON_STRUCT));
    memcpy(&common.prach_cnfg,  &sib2.rr_config_common_sib.prach_cnfg,  sizeof(LIBLTE_RRC_PRACH_CONFIG_SIB_STRUCT));
    if (sib2.rr_config_common_sib.srs_ul_cnfg.present) {
      memcpy(&common.srs_ul_cnfg,  &sib2.rr_config_common_sib.srs_ul_cnfg, sizeof(LIBLTE_RRC_SRS_UL_CONFIG_COMMON_STRUCT));
    } else {
      // default is release
      common.srs_ul_cnfg.present = false; 
    }
    phy->set_config_common(&common);

    phy->configure_ul_params();

    log_h->info("Set PUSCH ConfigCommon: HopOffset=%d, RSGroup=%d, RSNcs=%d, N_sb=%d\n",
                  sib2.rr_config_common_sib.pusch_cnfg.pusch_hopping_offset,
                  sib2.rr_config_common_sib.pusch_cnfg.ul_rs.group_assignment_pusch,
                  sib2.rr_config_common_sib.pusch_cnfg.ul_rs.cyclic_shift,
                  sib2.rr_config_common_sib.pusch_cnfg.n_sb);

    log_h->info("Set PUCCH ConfigCommon: DeltaShift=%d, CyclicShift=%d, N1=%d, NRB=%d\n",
                  liblte_rrc_delta_pucch_shift_num[sib2.rr_config_common_sib.pucch_cnfg.delta_pucch_shift],
                  sib2.rr_config_common_sib.pucch_cnfg.n_cs_an,
                  sib2.rr_config_common_sib.pucch_cnfg.n1_pucch_an,
                  sib2.rr_config_common_sib.pucch_cnfg.n_rb_cqi);
    
    log_h->info("Set PRACH ConfigCommon: SeqIdx=%d, HS=%d, FreqOffset=%d, ZC=%d, ConfigIndex=%d\n",
                  sib2.rr_config_common_sib.prach_cnfg.root_sequence_index,
                  sib2.rr_config_common_sib.prach_cnfg.prach_cnfg_info.high_speed_flag?1:0,
                  sib2.rr_config_common_sib.prach_cnfg.prach_cnfg_info.prach_freq_offset,
                  sib2.rr_config_common_sib.prach_cnfg.prach_cnfg_info.zero_correlation_zone_config,
                  sib2.rr_config_common_sib.prach_cnfg.prach_cnfg_info.prach_config_index);

    log_h->info("Set SRS ConfigCommon: BW-Configuration=%d, SF-Configuration=%d, ACKNACK=%d\n",
                  sib2.rr_config_common_sib.srs_ul_cnfg.bw_cnfg,
                  sib2.rr_config_common_sib.srs_ul_cnfg.subfr_cnfg,
                  sib2.rr_config_common_sib.srs_ul_cnfg.ack_nack_simul_tx);
    
  }
};



// Create classes
srslte::logger_file logger;
srslte::log_filter  log_phy;
srslte::log_filter  log_mac;
srslte::log_filter  log_rlc;
srslte::log_filter  log_tester;
srslte::mac_pcap    mac_pcap;
srsue::phy my_phy;
srsue::mac my_mac;
srslte::rlc rlc;
srslte::radio_multi my_radio; 

// Local classes for testing
tester my_tester; 


bool running = true; 

void sig_int_handler(int signo)
{
  running = false;
}

int main(int argc, char *argv[])
{
  
  parse_args(&prog_args, argc, argv);

  // set to null to disable pcap 
  const char *pcap_filename = "/tmp/ip_test.pcap"; 
  
  logger.init("/tmp/ip_test_ue.log");
  log_phy.init("PHY ", &logger, true);
  log_mac.init("MAC ", &logger, true);
  log_rlc.init("RLC ", &logger);
  log_tester.init("TEST", &logger);
  logger.log("\n\n");
 
  if (srsapps_verbose == 1) {
    log_phy.set_level(srslte::LOG_LEVEL_INFO);
    log_phy.set_hex_limit(100);
    log_mac.set_level(srslte::LOG_LEVEL_DEBUG);
    log_mac.set_hex_limit(100);
    log_rlc.set_level(srslte::LOG_LEVEL_DEBUG);
    log_rlc.set_hex_limit(1000);
    log_tester.set_level(srslte::LOG_LEVEL_DEBUG);
    log_tester.set_hex_limit(100);
    printf("Log level info\n");
  }
  if (srsapps_verbose == 2) {
    log_phy.set_level(srslte::LOG_LEVEL_DEBUG);
    log_phy.set_hex_limit(100);
    log_mac.set_level(srslte::LOG_LEVEL_DEBUG);
    log_mac.set_hex_limit(100);
    log_rlc.set_level(srslte::LOG_LEVEL_DEBUG);
    log_rlc.set_hex_limit(100);
    log_tester.set_level(srslte::LOG_LEVEL_DEBUG);
    log_tester.set_hex_limit(100);
    srslte_verbose = SRSLTE_VERBOSE_DEBUG;
    printf("Log level debug\n");
  }
    
  // Init Radio and PHY
#ifdef USE_RADIO
  my_radio.init();
#else
  my_radio.init(NULL, "dummy");
#endif
  
  my_radio.set_tx_freq(prog_args.tx_freq);
  my_radio.set_tx_gain(prog_args.tx_gain);
  my_radio.set_rx_freq(prog_args.rx_freq);
  my_radio.set_rx_gain(prog_args.rx_gain);
  if (prog_args.time_adv >= 0) {
    printf("Setting TA=%d samples\n",prog_args.time_adv); 
    my_radio.set_tx_adv(prog_args.time_adv);
  }
    
  my_phy.init(&my_radio, &my_mac, &my_tester, &log_phy, NULL);
  my_mac.init(&my_phy, &rlc, &my_tester, &log_mac);
  rlc.init(&my_tester, &my_tester, &my_tester, &log_rlc, &my_mac, 0 /* SRB0 */);
  my_tester.init(&my_phy, &my_mac, &rlc, &log_tester, prog_args.ip_address);

  
  if (pcap_filename) {
    mac_pcap.open(pcap_filename);
    my_mac.start_pcap(&mac_pcap);
    signal(SIGINT, sig_int_handler);     
  }
  
  // Set MAC defaults 
  LIBLTE_RRC_MAC_MAIN_CONFIG_STRUCT default_cfg;
  bzero(&default_cfg, sizeof(LIBLTE_RRC_MAC_MAIN_CONFIG_STRUCT));
  default_cfg.ulsch_cnfg.max_harq_tx        = LIBLTE_RRC_MAX_HARQ_TX_N5;
  default_cfg.ulsch_cnfg.periodic_bsr_timer = LIBLTE_RRC_PERIODIC_BSR_TIMER_INFINITY;
  default_cfg.ulsch_cnfg.retx_bsr_timer     = LIBLTE_RRC_RETRANSMISSION_BSR_TIMER_SF2560;  
  default_cfg.ulsch_cnfg.tti_bundling       = false;
  default_cfg.drx_cnfg.setup_present        = false;
  default_cfg.phr_cnfg.setup_present        = false;
  default_cfg.time_alignment_timer          = LIBLTE_RRC_TIME_ALIGNMENT_TIMER_INFINITY; 
  my_mac.set_config_main(&default_cfg);
  
  while(running) {
    if (my_tester.is_sib_received()) {
      printf("Main running\n");
      sleep(1);
    } else {
      my_tester.sib_search();
    }
  }
  
  if (pcap_filename) {
    mac_pcap.close();
  }
  
  my_phy.stop();
  my_mac.stop();
}




/******************* This is copied from srsue gw **********************/
int setup_if_addr(char *ip_addr)
{
    
  char *dev = (char*) "tun_srsue";

  // Construct the TUN device
  int tun_fd = open("/dev/net/tun", O_RDWR);
  if(0 > tun_fd)
  {
    perror("open");
    return(-1);
  }

  struct ifreq ifr;
  
  memset(&ifr, 0, sizeof(ifr));
  ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
  strncpy(ifr.ifr_ifrn.ifrn_name, dev, IFNAMSIZ);
  if(0 > ioctl(tun_fd, TUNSETIFF, &ifr))
  {
    perror("ioctl");
    return -1;
  }

  // Bring up the interface
  int sock = socket(AF_INET, SOCK_DGRAM, 0);
  if(0 > ioctl(sock, SIOCGIFFLAGS, &ifr))
  {
    perror("socket");
    return -1;
  }
  ifr.ifr_flags |= IFF_UP | IFF_RUNNING;
  if(0 > ioctl(sock, SIOCSIFFLAGS, &ifr))
  {
    perror("ioctl");
    return -1;
  }

  // Setup the IP address    
  sock                                                   = socket(AF_INET, SOCK_DGRAM, 0);
  ifr.ifr_addr.sa_family                                 = AF_INET;
  ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr = inet_addr(ip_addr);
  if(0 > ioctl(sock, SIOCSIFADDR, &ifr))
  {
    perror("ioctl");
    return -1;
  }
  ifr.ifr_netmask.sa_family                                 = AF_INET;
  ((struct sockaddr_in *)&ifr.ifr_netmask)->sin_addr.s_addr = inet_addr("255.255.255.0");
  if(0 > ioctl(sock, SIOCSIFNETMASK, &ifr))
  {
    perror("ioctl");
    return -1;
  }

  return(tun_fd);
}
