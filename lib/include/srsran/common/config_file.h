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

#ifndef SRSRAN_CONFIG_FILE_H
#define SRSRAN_CONFIG_FILE_H

#include "common.h"
#include "srsran/phy/common/phy_common.h"
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
    snprintf(full_path, sizeof(full_path), "%s/.config/srsran/%s", homedir, default_name.c_str());
    printf("Couldn't open %s, trying %s\n", filename.c_str(), full_path);
    filename = std::string(full_path);

    // try to open again
    conf.open(filename.c_str());
    if (conf.fail()) {
      // Last chance, try to find file in /etc/srsran
      ZERO_OBJECT(full_path);
      snprintf(full_path, sizeof(full_path), "/etc/srsran/%s", default_name.c_str());
      printf("Couldn't open %s either, trying %s\n", filename.c_str(), full_path);
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

#endif // SRSRAN_CONFIG_FILE_H
