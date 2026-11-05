# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: AGPL-3.0-or-later

set(SOURCES sched_base.cc sched_time_rr.cc sched_time_pf.cc)
add_library(mac_schedulers OBJECT ${SOURCES})
