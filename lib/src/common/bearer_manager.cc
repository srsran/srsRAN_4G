/**
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
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
  auto                       user_it = users_map.find(rnti);
  if (user_it == users_map.end()) {
    // add empty bearer map
    auto p  = users_map.emplace(rnti, user_bearers{});
    user_it = p.first;
  }

  auto bearer_it = user_it->second.bearers.find(eps_bearer_id);
  if (bearer_it != user_it->second.bearers.end()) {
    logger.error("EPS bearer ID %d already registered", eps_bearer_id);
    return;
  }
  user_it->second.bearers.emplace(eps_bearer_id, radio_bearer_t{rat, lcid, eps_bearer_id});
  user_it->second.lcid_to_eps_bearer_id.emplace(lcid, eps_bearer_id);
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

  auto bearer_it = user_it->second.bearers.find(eps_bearer_id);
  if (bearer_it == user_it->second.bearers.end()) {
    logger.error("Can't remove EPS bearer ID %d", eps_bearer_id);
    return;
  }
  uint32_t lcid = bearer_it->second.lcid;
  user_it->second.bearers.erase(bearer_it);
  user_it->second.lcid_to_eps_bearer_id.erase(lcid);
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

  user_it->second.lcid_to_eps_bearer_id.clear();
  user_it->second.bearers.clear();
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

  return user_it->second.bearers.find(eps_bearer_id) != user_it->second.bearers.end();
}

// Stack interface
bearer_manager::radio_bearer_t bearer_manager::get_radio_bearer(uint32_t eps_bearer_id)
{
  return get_radio_bearer(default_key, eps_bearer_id);
}

bearer_manager::radio_bearer_t bearer_manager::get_lcid_bearer(uint16_t rnti, uint32_t lcid)
{
  srsran::rwlock_read_guard rw_lock(rwlock);

  auto user_it = users_map.find(rnti);
  if (user_it == users_map.end()) {
    return invalid_rb;
  }

  auto lcid_it = user_it->second.lcid_to_eps_bearer_id.find(lcid);
  if (lcid_it != user_it->second.lcid_to_eps_bearer_id.end()) {
    return user_it->second.bearers.at(lcid_it->second);
  }
  return invalid_rb;
}

bearer_manager::radio_bearer_t bearer_manager::get_radio_bearer(uint16_t rnti, uint32_t eps_bearer_id)
{
  srsran::rwlock_read_guard rw_lock(rwlock);

  auto user_it = users_map.find(rnti);
  if (user_it == users_map.end()) {
    return invalid_rb;
  }

  if (user_it->second.bearers.find(eps_bearer_id) != user_it->second.bearers.end()) {
    return user_it->second.bearers.at(eps_bearer_id);
  }
  return invalid_rb;
}

} // namespace srsran