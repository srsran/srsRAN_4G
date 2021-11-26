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

#include "srsran/common/bearer_manager.h"

namespace srsran {

namespace detail {

const ue_bearer_manager_impl::radio_bearer_t ue_bearer_manager_impl::invalid_rb{srsran::srsran_rat_t::nulltype, 0, 0};

bool ue_bearer_manager_impl::add_eps_bearer(uint8_t eps_bearer_id, srsran::srsran_rat_t rat, uint32_t lcid)
{
  auto bearer_it = bearers.find(eps_bearer_id);
  if (bearer_it != bearers.end()) {
    return false;
  }
  bearers.emplace(eps_bearer_id, radio_bearer_t{rat, lcid, eps_bearer_id});
  lcid_to_eps_bearer_id.emplace(lcid, eps_bearer_id);
  return true;
}

bool ue_bearer_manager_impl::remove_eps_bearer(uint8_t eps_bearer_id)
{
  auto bearer_it = bearers.find(eps_bearer_id);
  if (bearer_it == bearers.end()) {
    return false;
  }
  uint32_t lcid = bearer_it->second.lcid;
  bearers.erase(bearer_it);
  lcid_to_eps_bearer_id.erase(lcid);
  return true;
}

void ue_bearer_manager_impl::reset()
{
  lcid_to_eps_bearer_id.clear();
  bearers.clear();
}

bool ue_bearer_manager_impl::has_active_radio_bearer(uint32_t eps_bearer_id)
{
  return bearers.count(eps_bearer_id) > 0;
}

ue_bearer_manager_impl::radio_bearer_t ue_bearer_manager_impl::get_radio_bearer(uint32_t eps_bearer_id)
{
  auto it = bearers.find(eps_bearer_id);
  return it != bearers.end() ? it->second : invalid_rb;
}

ue_bearer_manager_impl::radio_bearer_t ue_bearer_manager_impl::get_eps_bearer_id_for_lcid(uint32_t lcid)
{
  auto lcid_it = lcid_to_eps_bearer_id.find(lcid);
  return lcid_it != lcid_to_eps_bearer_id.end() ? bearers.at(lcid_it->second) : invalid_rb;
}

} // namespace detail
} // namespace srsran

namespace srsue {

ue_bearer_manager::ue_bearer_manager() : logger(srslog::fetch_basic_logger("STCK", false))
{
  pthread_rwlock_init(&rwlock, nullptr);
}

ue_bearer_manager::~ue_bearer_manager()
{
  pthread_rwlock_destroy(&rwlock);
}

void ue_bearer_manager::add_eps_bearer(uint8_t eps_bearer_id, srsran::srsran_rat_t rat, uint32_t lcid)
{
  srsran::rwlock_write_guard rw_lock(rwlock);
  if (impl.add_eps_bearer(eps_bearer_id, rat, lcid)) {
    logger.info(
        "Bearers: Registered EPS bearer ID %d for lcid=%d over %s-PDCP", eps_bearer_id, lcid, to_string(rat).c_str());
  } else {
    logger.warning("Bearers: EPS bearer ID %d already registered", eps_bearer_id);
  }
}

void ue_bearer_manager::remove_eps_bearer(uint8_t eps_bearer_id)
{
  srsran::rwlock_write_guard rw_lock(rwlock);
  if (impl.remove_eps_bearer(eps_bearer_id)) {
    logger.info("Bearers: Removed mapping for EPS bearer ID %d", eps_bearer_id);
  } else {
    logger.error("Bearers: Can't remove EPS bearer ID %d", eps_bearer_id);
  }
}

void ue_bearer_manager::reset()
{
  srsran::rwlock_write_guard rw_lock(rwlock);
  impl.reset();
  logger.info("Bearers: Reset EPS bearer manager");
}

} // namespace srsue

namespace srsenb {

enb_bearer_manager::enb_bearer_manager() : logger(srslog::fetch_basic_logger("STCK", false)) {}

enb_bearer_manager::~enb_bearer_manager() {}

void enb_bearer_manager::add_eps_bearer(uint16_t rnti, uint8_t eps_bearer_id, srsran::srsran_rat_t rat, uint32_t lcid)
{
  auto user_it = users_map.find(rnti);
  if (user_it == users_map.end()) {
    // add empty bearer map
    // users_map.emplace( )   returns pair<iterator,bool>
    auto p = users_map.emplace(rnti, srsran::detail::ue_bearer_manager_impl{});
    if (!p.second) {
      logger.error("Bearers: Unable to add a new bearer map for rnti=0x%x", rnti);
      return;
    }
    user_it = p.first;
  }

  if (user_it->second.add_eps_bearer(eps_bearer_id, rat, lcid)) {
    logger.info("Bearers: Registered eps-BearerID=%d for rnti=0x%x, lcid=%d over %s-PDCP",
                eps_bearer_id,
                rnti,
                lcid,
                to_string(rat).c_str());
  } else {
    logger.warning("Bearers: EPS bearer ID %d for rnti=0x%x already registered", eps_bearer_id, rnti);
  }
}

void enb_bearer_manager::remove_eps_bearer(uint16_t rnti, uint8_t eps_bearer_id)
{
  auto user_it = users_map.find(rnti);
  if (user_it == users_map.end()) {
    logger.info("Bearers: No EPS bearer registered for rnti=0x%x", rnti);
    return;
  }

  if (user_it->second.remove_eps_bearer(eps_bearer_id)) {
    logger.info("Bearers: Removed mapping for EPS bearer ID %d for rnti=0x%x", eps_bearer_id, rnti);
  } else {
    logger.info("Bearers: Can't remove EPS bearer ID %d, rnti=0x%x", eps_bearer_id, rnti);
  }
}

void enb_bearer_manager::rem_user(uint16_t rnti)
{
  auto user_it = users_map.find(rnti);
  if (user_it == users_map.end()) {
    logger.info("Bearers: No EPS bearer registered for rnti=0x%x", rnti);
    return;
  }

  logger.info("Bearers: Removed rnti=0x%x from EPS bearer manager", rnti);
  users_map.erase(user_it);
}

bool enb_bearer_manager::has_active_radio_bearer(uint16_t rnti, uint32_t eps_bearer_id)
{
  auto user_it = users_map.find(rnti);
  if (user_it == users_map.end()) {
    return false;
  }
  return user_it->second.has_active_radio_bearer(eps_bearer_id);
}

enb_bearer_manager::radio_bearer_t enb_bearer_manager::get_lcid_bearer(uint16_t rnti, uint32_t lcid)
{
  auto user_it = users_map.find(rnti);
  if (user_it == users_map.end()) {
    return srsran::detail::ue_bearer_manager_impl::invalid_rb;
  }
  return user_it->second.get_eps_bearer_id_for_lcid(lcid);
}

enb_bearer_manager::radio_bearer_t enb_bearer_manager::get_radio_bearer(uint16_t rnti, uint32_t eps_bearer_id)
{
  auto user_it = users_map.find(rnti);
  if (user_it == users_map.end()) {
    return srsran::detail::ue_bearer_manager_impl::invalid_rb;
  }
  return user_it->second.get_radio_bearer(eps_bearer_id);
}

} // namespace srsenb