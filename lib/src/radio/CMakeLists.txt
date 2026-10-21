# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

if(RF_FOUND)
  add_library(srsran_radio STATIC radio.cc channel_mapping.cc)
  target_link_libraries(srsran_radio srsran_rf srsran_common)
  install(TARGETS srsran_radio DESTINATION ${LIBRARY_DIR} OPTIONAL)
endif(RF_FOUND)

add_subdirectory(test)
