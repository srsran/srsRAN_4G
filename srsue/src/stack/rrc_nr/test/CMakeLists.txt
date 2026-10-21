# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

add_executable(ue_rrc_nr_test ue_rrc_nr_test.cc)
target_link_libraries(ue_rrc_nr_test srsue_rrc srsue_rrc_nr srsue_upper srsran_common srsran_pdcp srsran_phy rrc_asn1 rrc_nr_asn1)