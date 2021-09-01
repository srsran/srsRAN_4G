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

bearer_manager::bearer_manager() : logger(srslog::fetch_basic_logger("STCK", false))
{
  pthread_rwlock_init(&rwlock, nullptr);
}

bearer_manager::~bearer_manager()
{
  pthread_rwlock_destroy(&rwlock);
}

void bearer_manager::add_eps_bearer(uint8_t eps_bearer_id, srsran::srsran_rat_t rat, uint32_t lcid)
{
  add_eps_bearer(default_key, eps_bearer_id, rat, lcid);
}

void bearer_manager::add_eps_bearer(uint16_t rnti, uint8_t eps_bearer_id, srsran::srsran_rat_t rat, uint32_t lcid)
{
  srsran::rwlock_write_guard rw_lock(rwlock);
  auto user_it = users_map.find(rnti);
  if (user_it == users_map.end()) {
    // add empty bearer map
    users_map.emplace(rnti, eps_rb_map_t{});
    user_it = users_map.find(rnti);
  }

  auto bearer_it = user_it->second.find(eps_bearer_id);
  if (bearer_it != user_it->second.end()) {
    logger.error("EPS bearer ID %d already registered", eps_bearer_id);
    return;
  }
  user_it->second.emplace(eps_bearer_id, radio_bearer_t{rat, lcid});
  logger.info("Registered EPS bearer ID %d for lcid=%d over %s-PDCP", eps_bearer_id, lcid, to_string(rat).c_str());
}

void bearer_manager::remove_eps_bearer(uint8_t eps_bearer_id)
{
  remove_eps_bearer(default_key, eps_bearer_id);
}

void bearer_manager::remove_eps_bearer(uint16_t rnti, uint8_t eps_bearer_id)
{
  srsran::rwlock_write_guard rw_lock(rwlock);

  auto user_it = users_map.find(rnti);
  if (user_it == users_map.end()) {
    logger.error("No EPS bearer registered for rnti=%x", rnti);
    return;
  }

  auto bearer_it = user_it->second.find(eps_bearer_id);
  if (bearer_it == user_it->second.end()) {
    logger.error("Can't remove EPS bearer ID %d", eps_bearer_id);
    return;
  }
  user_it->second.erase(bearer_it);
  logger.info("Removed mapping for EPS bearer ID %d", eps_bearer_id);
}

void bearer_manager::reset()
{
  reset(default_key);
}

void bearer_manager::reset(uint16_t rnti)
{
  srsran::rwlock_write_guard rw_lock(rwlock);

  auto user_it = users_map.find(rnti);
  if (user_it == users_map.end()) {
    logger.error("No EPS bearer registered for rnti=%x", rnti);
    return;
  }

  user_it->second.clear();
  logger.info("Reset EPS bearer manager");
}

// GW interface
bool bearer_manager::has_active_radio_bearer(uint32_t eps_bearer_id)
{
  return has_active_radio_bearer(default_key, eps_bearer_id);
}

bool bearer_manager::has_active_radio_bearer(uint16_t rnti, uint32_t eps_bearer_id)
{
  srsran::rwlock_read_guard rw_lock(rwlock);

  auto user_it = users_map.find(rnti);
  if (user_it == users_map.end()) {
    return false;
  }

  return user_it->second.find(eps_bearer_id) != user_it->second.end();
}

// Stack interface
bearer_manager::radio_bearer_t bearer_manager::get_radio_bearer(uint32_t eps_bearer_id)
{
  return get_radio_bearer(default_key, eps_bearer_id);
}

bearer_manager::radio_bearer_t bearer_manager::get_radio_bearer(uint16_t rnti, uint32_t eps_bearer_id)
{
  srsran::rwlock_read_guard rw_lock(rwlock);

  auto user_it = users_map.find(rnti);
  if (user_it == users_map.end()) {
    return invalid_rb;
  }

  if (user_it->second.find(eps_bearer_id) != user_it->second.end()) {
    return user_it->second.at(eps_bearer_id);
  }
  return invalid_rb;
}

} // namespace srsue