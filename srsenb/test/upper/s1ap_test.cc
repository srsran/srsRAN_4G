/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsenb/hdr/stack/upper/s1ap.h"
#include "srsenb/test/common/dummy_classes.h"
#include "srsran/common/network_utils.h"
#include "srsran/common/test_common.h"

using namespace srsenb;

struct mme_dummy {
  mme_dummy(const char* addr_str_, int port_) : addr_str(addr_str_), port(port_)
  {
    srsran::net_utils::set_sockaddr(&mme_sockaddr, addr_str, port);
    {
      using namespace srsran::net_utils;
      fd = open_socket(addr_family::ipv4, socket_type::seqpacket, protocol_type::SCTP);
      TESTASSERT(fd > 0);
      TESTASSERT(bind_addr(fd, mme_sockaddr));
    }

    srsran_assert(listen(fd, SOMAXCONN) == 0, "Failed to listen to incoming SCTP connections");
  }

  ~mme_dummy()
  {
    if (fd > 0) {
      close(fd);
    }
  }

  srsran::unique_byte_buffer_t read_msg(sockaddr_in* sockfrom = nullptr)
  {
    srsran::unique_byte_buffer_t pdu     = srsran::make_byte_buffer();
    sockaddr_in                  from    = {};
    socklen_t                    fromlen = sizeof(from);
    sctp_sndrcvinfo              sri     = {};
    int                          flags   = 0;
    ssize_t n_recv = sctp_recvmsg(fd, pdu->msg, pdu->get_tailroom(), (struct sockaddr*)&from, &fromlen, &sri, &flags);
    if (n_recv > 0) {
      if (sockfrom != nullptr) {
        *sockfrom = from;
      }
      pdu->N_bytes = n_recv;
    }
    return pdu;
  }

  const char*                  addr_str;
  int                          port;
  struct sockaddr_in           mme_sockaddr = {};
  int                          fd;
  srsran::unique_byte_buffer_t last_sdu;
};

struct dummy_socket_manager : public srsran::socket_manager_itf {
  dummy_socket_manager() : srsran::socket_manager_itf(srslog::fetch_basic_logger("TEST")) {}

  /// Register (fd, callback). callback is called within socket thread when fd has data.
  bool add_socket_handler(int fd, recv_callback_t handler) final
  {
    if (s1u_fd > 0) {
      return false;
    }
    s1u_fd   = fd;
    callback = std::move(handler);
    return true;
  }

  /// remove registered socket fd
  bool remove_socket(int fd) final
  {
    if (s1u_fd < 0) {
      return false;
    }
    s1u_fd = -1;
    return true;
  }

  int             s1u_fd;
  recv_callback_t callback;
};

struct rrc_tester : public rrc_dummy {
  void modify_erabs(uint16_t                                 rnti,
                    const asn1::s1ap::erab_modify_request_s& msg,
                    std::vector<uint16_t>*                   erabs_modified,
                    std::vector<uint16_t>*                   erabs_failed_to_modify) override
  {
    *erabs_modified         = next_erabs_modified;
    *erabs_failed_to_modify = next_erabs_failed_to_modify;
  }
  void release_ue(uint16_t rnti) override { last_released_rnti = rnti; }

  uint16_t              last_released_rnti = SRSRAN_INVALID_RNTI;
  std::vector<uint16_t> next_erabs_modified, next_erabs_failed_to_modify;
};

void run_s1_setup(s1ap& s1ap_obj, mme_dummy& mme)
{
  asn1::s1ap::s1ap_pdu_c s1ap_pdu;

  // eNB -> MME: S1 Setup Request
  srsran::unique_byte_buffer_t sdu = mme.read_msg();
  TESTASSERT(sdu->N_bytes > 0);
  asn1::cbit_ref cbref(sdu->msg, sdu->N_bytes);
  TESTASSERT(s1ap_pdu.unpack(cbref) == asn1::SRSASN_SUCCESS);
  TESTASSERT(s1ap_pdu.type().value == asn1::s1ap::s1ap_pdu_c::types_opts::init_msg);
  TESTASSERT(s1ap_pdu.init_msg().proc_code == ASN1_S1AP_ID_S1_SETUP);

  // MME -> eNB: S1 Setup Response
  sockaddr_in     mme_addr        = {};
  sctp_sndrcvinfo rcvinfo         = {};
  int             flags           = 0;
  uint8_t         s1_setup_resp[] = {0x20, 0x11, 0x00, 0x25, 0x00, 0x00, 0x03, 0x00, 0x3d, 0x40, 0x0a, 0x03, 0x80, 0x73,
                             0x72, 0x73, 0x6d, 0x6d, 0x65, 0x30, 0x31, 0x00, 0x69, 0x00, 0x0b, 0x00, 0x00, 0x00,
                             0xf1, 0x10, 0x00, 0x00, 0x01, 0x00, 0x00, 0x1a, 0x00, 0x57, 0x40, 0x01, 0xff};
  memcpy(sdu->msg, s1_setup_resp, sizeof(s1_setup_resp));
  sdu->N_bytes = sizeof(s1_setup_resp);
  TESTASSERT(s1ap_obj.handle_mme_rx_msg(std::move(sdu), mme_addr, rcvinfo, flags));
}

void add_rnti(s1ap& s1ap_obj, mme_dummy& mme)
{
  asn1::s1ap::s1ap_pdu_c s1ap_pdu;

  // New UE
  uint8_t nas_msg[] = {0x00, 0x1a, 0x00, 0x21, 0x20, 0x17, 0x82, 0xa8, 0x64, 0x46, 0x04, 0x07, 0x41,
                       0x01, 0x0b, 0xf6, 0x00, 0xf1, 0x10, 0x00, 0x01, 0x1a, 0x5e, 0xa4, 0x54, 0x47,
                       0x02, 0xf0, 0x70, 0x00, 0x04, 0x02, 0x01, 0xd0, 0x11, 0x91, 0xe0};

  srsran::unique_byte_buffer_t sdu = srsran::make_byte_buffer();
  memcpy(sdu->msg, nas_msg, sizeof(nas_msg));
  sdu->N_bytes = sizeof(nas_msg);
  s1ap_obj.initial_ue(0x46, 0, asn1::s1ap::rrc_establishment_cause_opts::mo_data, std::move(sdu));
  sdu = mme.read_msg();
  TESTASSERT(sdu->N_bytes > 0);
  asn1::cbit_ref cbref{sdu->msg, sdu->N_bytes};
  TESTASSERT(s1ap_pdu.unpack(cbref) == SRSRAN_SUCCESS);
  TESTASSERT(s1ap_pdu.type().value == asn1::s1ap::s1ap_pdu_c::types_opts::init_msg);
  TESTASSERT(s1ap_pdu.init_msg().proc_code == ASN1_S1AP_ID_INIT_UE_MSG);

  // InitialContextSetupRequest (skip all NAS exchange)
  sockaddr_in     mme_addr   = {};
  sctp_sndrcvinfo rcvinfo    = {};
  int             flags      = 0;
  uint8_t         icsr_msg[] = {
      0x00, 0x09, 0x00, 0x80, 0xac, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x02, 0x00, 0x01, 0x00, 0x08, 0x00, 0x02,
      0x00, 0x01, 0x00, 0x42, 0x00, 0x0a, 0x18, 0x3b, 0x9a, 0xca, 0x00, 0x60, 0x3b, 0x9a, 0xca, 0x00, 0x00, 0x18,
      0x00, 0x5e, 0x00, 0x00, 0x34, 0x00, 0x59, 0x45, 0x00, 0x09, 0x3c, 0x0f, 0x80, 0x7f, 0x00, 0x01, 0x64, 0x00,
      0x00, 0x00, 0x01, 0x4a, 0x27, 0x9b, 0x6d, 0xe9, 0x42, 0x01, 0x07, 0x42, 0x01, 0x3e, 0x06, 0x00, 0x00, 0xf1,
      0x10, 0x00, 0x07, 0x00, 0x1d, 0x52, 0x01, 0xc1, 0x01, 0x09, 0x07, 0x06, 0x73, 0x72, 0x73, 0x61, 0x70, 0x6e,
      0x05, 0x01, 0xc0, 0xa8, 0x0a, 0x02, 0x27, 0x08, 0x80, 0x00, 0x0d, 0x04, 0x08, 0x08, 0x08, 0x08, 0x50, 0x0b,
      0xf6, 0x00, 0xf1, 0x10, 0x00, 0x01, 0x1a, 0x32, 0xdd, 0x59, 0x35, 0x13, 0x00, 0xf1, 0x10, 0x00, 0x01, 0x23,
      0x05, 0xf4, 0x32, 0xdd, 0x59, 0x35, 0x00, 0x6b, 0x00, 0x05, 0x18, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x49, 0x00,
      0x20, 0x84, 0xa4, 0xea, 0x15, 0x55, 0xb3, 0xe0, 0xf4, 0x55, 0xbe, 0x1f, 0x41, 0x52, 0x92, 0xfc, 0x04, 0xd8,
      0x02, 0x38, 0x0d, 0xe0, 0x81, 0x29, 0xe1, 0xaa, 0xd7, 0xc4, 0x7b, 0x12, 0x95, 0x72, 0xbe};
  sdu = srsran::make_byte_buffer();
  memcpy(sdu->msg, icsr_msg, sizeof(icsr_msg));
  sdu->N_bytes = sizeof(icsr_msg);
  TESTASSERT(s1ap_obj.handle_mme_rx_msg(std::move(sdu), mme_addr, rcvinfo, flags));

  // InitialContextSetupResponse
  uint8_t icsresp[] = {0x20, 0x09, 0x00, 0x22, 0x00, 0x00, 0x03, 0x00, 0x00, 0x40, 0x02, 0x00, 0x01,
                       0x00, 0x08, 0x40, 0x02, 0x00, 0x01, 0x00, 0x33, 0x40, 0x0f, 0x00, 0x00, 0x32,
                       0x40, 0x0a, 0x0a, 0x1f, 0x7f, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01};
  cbref             = asn1::cbit_ref(icsresp, sizeof(icsresp));
  TESTASSERT(s1ap_pdu.unpack(cbref) == SRSRAN_SUCCESS);
  s1ap_obj.ue_ctxt_setup_complete(0x46, s1ap_pdu.successful_outcome().value.init_context_setup_resp());
  sdu = mme.read_msg();
  TESTASSERT(sdu->N_bytes > 0);
  cbref = asn1::cbit_ref{sdu->msg, sdu->N_bytes};
  TESTASSERT(s1ap_pdu.unpack(cbref) == SRSRAN_SUCCESS);
  TESTASSERT(s1ap_pdu.type().value == asn1::s1ap::s1ap_pdu_c::types_opts::successful_outcome);
  TESTASSERT(s1ap_pdu.successful_outcome().proc_code == ASN1_S1AP_ID_INIT_CONTEXT_SETUP);
}

enum class test_event { success, wrong_erabid_mod, wrong_mme_s1ap_id };

void test_s1ap_erab_setup(test_event event)
{
  srsran::task_scheduler       task_sched;
  srslog::basic_logger&        logger = srslog::fetch_basic_logger("S1AP");
  dummy_socket_manager         rx_sockets;
  s1ap                         s1ap_obj(&task_sched, logger, &rx_sockets);
  rrc_tester                   rrc;
  asn1::s1ap::s1ap_pdu_c       s1ap_pdu;
  srsran::unique_byte_buffer_t sdu;

  const char*    mme_addr_str = "127.0.0.1";
  const uint32_t MME_PORT     = 36412;
  mme_dummy      mme(mme_addr_str, MME_PORT);

  s1ap_args_t args   = {};
  args.cell_id       = 0x01;
  args.enb_id        = 0x19B;
  args.mcc           = 907;
  args.mnc           = 70;
  args.s1c_bind_addr = "127.0.0.100";
  args.tac           = 7;
  args.gtp_bind_addr = "127.0.0.100";
  args.mme_addr      = mme_addr_str;
  args.enb_name      = "srsenb01";

  TESTASSERT(s1ap_obj.init(args, &rrc) == SRSRAN_SUCCESS);

  run_s1_setup(s1ap_obj, mme);
  add_rnti(s1ap_obj, mme);

  // E-RAB Modify Request
  sockaddr_in     mme_addr      = {};
  sctp_sndrcvinfo rcvinfo       = {};
  int             flags         = 0;
  uint8_t         mod_req_msg[] = {0x00, 0x06, 0x00, 0x1E, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x02, 0x00,
                           0x01, 0x00, 0x08, 0x00, 0x02, 0x00, 0x01, 0x00, 0x1E, 0x00, 0x0B, 0x00,
                           0x00, 0x24, 0x00, 0x06, 0x0A, 0x00, 0x09, 0x3C, 0x01, 0x00};
  // 00 06 00 1E 00 00 03 00 00 00 02 00 01 00 08 00 02 00 01 00 1E 00 0B 00 00 24 00 06 0A 00 09 3C 01 00
  if (event == test_event::wrong_erabid_mod) {
    mod_req_msg[sizeof(mod_req_msg) - 6] = 0x0C; // E-RAB id = 6
    rrc.next_erabs_failed_to_modify.push_back(6);
  } else if (event == test_event::wrong_mme_s1ap_id) {
    mod_req_msg[12] = 0x02; // MME-UE-S1AP-ID = 2
  } else {
    rrc.next_erabs_modified.push_back(5);
  }
  sdu = srsran::make_byte_buffer();
  memcpy(sdu->msg, mod_req_msg, sizeof(mod_req_msg));
  sdu->N_bytes = sizeof(mod_req_msg);
  TESTASSERT(rrc.last_released_rnti == SRSRAN_INVALID_RNTI);
  TESTASSERT(s1ap_obj.handle_mme_rx_msg(std::move(sdu), mme_addr, rcvinfo, flags));
  sdu = mme.read_msg();
  TESTASSERT(sdu->N_bytes > 0);
  asn1::cbit_ref cbref{sdu->msg, sdu->N_bytes};
  TESTASSERT(s1ap_pdu.unpack(cbref) == SRSRAN_SUCCESS);

  if (event == test_event::wrong_mme_s1ap_id) {
    // See TS 36.413, Section 10.6 - Handling of AP ID
    TESTASSERT(s1ap_pdu.type().value == asn1::s1ap::s1ap_pdu_c::types_opts::init_msg);
    TESTASSERT(s1ap_pdu.init_msg().proc_code == ASN1_S1AP_ID_ERROR_IND);
    auto& protocol_ies = s1ap_pdu.init_msg().value.error_ind().protocol_ies;
    TESTASSERT(protocol_ies.mme_ue_s1ap_id_present and protocol_ies.mme_ue_s1ap_id.value.value == 2);
    TESTASSERT(protocol_ies.enb_ue_s1ap_id_present and protocol_ies.enb_ue_s1ap_id.value.value == 1);
    TESTASSERT(rrc.last_released_rnti == 0x46);
    return;
  }

  TESTASSERT(s1ap_pdu.type().value == asn1::s1ap::s1ap_pdu_c::types_opts::successful_outcome);
  TESTASSERT(s1ap_pdu.successful_outcome().proc_code == ASN1_S1AP_ID_ERAB_MODIFY);
  auto& protocol_ies = s1ap_pdu.successful_outcome().value.erab_modify_resp().protocol_ies;
  if (event == test_event::wrong_erabid_mod) {
    TESTASSERT(not protocol_ies.erab_modify_list_bearer_mod_res_present);
    TESTASSERT(protocol_ies.erab_failed_to_modify_list_present);
    TESTASSERT(protocol_ies.erab_failed_to_modify_list.value.size() == 1);
    auto& erab_item = protocol_ies.erab_failed_to_modify_list.value[0].value.erab_item();
    TESTASSERT(erab_item.erab_id == 6);
    TESTASSERT(erab_item.cause.type().value == asn1::s1ap::cause_c::types_opts::radio_network);
    TESTASSERT(erab_item.cause.radio_network().value == asn1::s1ap::cause_radio_network_opts::unknown_erab_id);
    return;
  }

  TESTASSERT(protocol_ies.erab_modify_list_bearer_mod_res_present);
  TESTASSERT(not protocol_ies.erab_failed_to_modify_list_present);
  TESTASSERT(protocol_ies.erab_modify_list_bearer_mod_res.value.size() == 1);
  auto& erab_item = protocol_ies.erab_modify_list_bearer_mod_res.value[0].value.erab_modify_item_bearer_mod_res();
  TESTASSERT(erab_item.erab_id == 5);
}

int main(int argc, char** argv)
{
  // Setup logging.
  auto& logger = srslog::fetch_basic_logger("S1AP");
  logger.set_level(srslog::basic_levels::debug);
  logger.set_hex_dump_max_size(-1);

  // Start the log backend.
  srsran::test_init(argc, argv);

  test_s1ap_erab_setup(test_event::success);
  test_s1ap_erab_setup(test_event::wrong_erabid_mod);
  test_s1ap_erab_setup(test_event::wrong_mme_s1ap_id);
}