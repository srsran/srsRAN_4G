# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

set(SOURCES ngap.cc ngap_ue.cc ngap_ue_proc.cc ngap_ue_bearer_manager.cc)
add_library(srsgnb_ngap STATIC ${SOURCES})

add_subdirectory(test)

