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

#ifndef SRSLTE_CONFIG_FILE_H
#define SRSLTE_CONFIG_FILE_H

#include "common.h"
#include <fstream>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

bool config_exists(std::string& filename, std::string default_name)
{
  std::ifstream conf(filename.c_str(), std::ios::in);
  if (conf.fail()) {
    // try config folder instead
    const char* homedir = NULL;
    char        full_path[256];
    ZERO_OBJECT(full_path);
    if ((homedir = getenv("HOME")) == NULL) {
      homedir = getpwuid(getuid())->pw_dir;
    }
    if (!homedir) {
      homedir = ".";
    }
    snprintf(full_path, sizeof(full_path), "%s/.config/srslte/%s", homedir, default_name.c_str());
    filename = std::string(full_path);

    // try to open again
    conf.open(filename.c_str());
    if (conf.fail()) {
      // Last chance, try to find file in /etc/srslte
      ZERO_OBJECT(full_path);
      snprintf(full_path, sizeof(full_path), "/etc/srslte/%s", default_name.c_str());
      filename = std::string(full_path);

      // try to open again
      conf.open(filename.c_str());
      if (conf.fail()) {
        return false;
      }
    }
  }
  return true;
}

#endif // SRSLTE_CONFIG_FILE_H
