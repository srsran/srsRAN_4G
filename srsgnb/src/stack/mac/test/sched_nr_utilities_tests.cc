//
// Created by carlo on 9/12/21.
//

#include "srsran/common/test_common.h"
extern "C" {
#include "srsran/phy/phch/ra_nr.h"
}

void test_cqi_to_mcs()
{
  // Sample random CQI values and test the returned MCS, for different combinations of CQI and MCS tables
  // Set the parameters so that to select a given MCS table (1, 2, or 3)

  //  TEST CQI Table 1 - MCS table 1
  int mcs = srsran_ra_nr_cqi_to_mcs(/* cqi */ 3,
                                    /* cqi_table_idx */ SRSRAN_CSI_CQI_TABLE_1,
                                    /* mcs_table */ srsran_mcs_table_64qam,
                                    /* dci_format */ srsran_dci_format_nr_1_0,
                                    /* search_space_type*/ srsran_search_space_type_ue,
                                    /* rnti_type */ srsran_rnti_type_c);
  TESTASSERT_EQ(2, mcs);

  mcs = srsran_ra_nr_cqi_to_mcs(5,
                                SRSRAN_CSI_CQI_TABLE_1,
                                srsran_mcs_table_64qam,
                                srsran_dci_format_nr_1_0,
                                srsran_search_space_type_ue,
                                srsran_rnti_type_c);
  TESTASSERT_EQ(6, mcs);

  mcs = srsran_ra_nr_cqi_to_mcs(9,
                                SRSRAN_CSI_CQI_TABLE_1,
                                srsran_mcs_table_64qam,
                                srsran_dci_format_nr_1_0,
                                srsran_search_space_type_ue,
                                srsran_rnti_type_c);
  TESTASSERT_EQ(15, mcs);

  mcs = srsran_ra_nr_cqi_to_mcs(12,
                                SRSRAN_CSI_CQI_TABLE_1,
                                srsran_mcs_table_64qam,
                                srsran_dci_format_nr_1_0,
                                srsran_search_space_type_ue,
                                srsran_rnti_type_c);
  TESTASSERT_EQ(22, mcs);

  //  TEST CQI Table 2 - MCS table 2
  mcs = srsran_ra_nr_cqi_to_mcs(1,
                                SRSRAN_CSI_CQI_TABLE_2,
                                srsran_mcs_table_256qam,
                                srsran_dci_format_nr_1_1,
                                srsran_search_space_type_ue,
                                srsran_rnti_type_c);
  TESTASSERT_EQ(0, mcs);

  mcs = srsran_ra_nr_cqi_to_mcs(4,
                                SRSRAN_CSI_CQI_TABLE_2,
                                srsran_mcs_table_256qam,
                                srsran_dci_format_nr_1_1,
                                srsran_search_space_type_ue,
                                srsran_rnti_type_c);
  TESTASSERT_EQ(5, mcs);

  mcs = srsran_ra_nr_cqi_to_mcs(7,
                                SRSRAN_CSI_CQI_TABLE_2,
                                srsran_mcs_table_256qam,
                                srsran_dci_format_nr_1_1,
                                srsran_search_space_type_ue,
                                srsran_rnti_type_c);
  TESTASSERT_EQ(11, mcs);

  mcs = srsran_ra_nr_cqi_to_mcs(11,
                                SRSRAN_CSI_CQI_TABLE_2,
                                srsran_mcs_table_256qam,
                                srsran_dci_format_nr_1_1,
                                srsran_search_space_type_ue,
                                srsran_rnti_type_c);
  TESTASSERT_EQ(19, mcs);

  //  TEST CQI Table 3 - MCS table 3
  mcs = srsran_ra_nr_cqi_to_mcs(2,
                                SRSRAN_CSI_CQI_TABLE_3,
                                srsran_mcs_table_qam64LowSE,
                                srsran_dci_format_nr_1_0,
                                srsran_search_space_type_ue,
                                srsran_rnti_type_c);
  TESTASSERT_EQ(2, mcs);

  mcs = srsran_ra_nr_cqi_to_mcs(8,
                                SRSRAN_CSI_CQI_TABLE_3,
                                srsran_mcs_table_qam64LowSE,
                                srsran_dci_format_nr_1_0,
                                srsran_search_space_type_ue,
                                srsran_rnti_type_c);
  TESTASSERT_EQ(14, mcs);

  mcs = srsran_ra_nr_cqi_to_mcs(13,
                                SRSRAN_CSI_CQI_TABLE_3,
                                srsran_mcs_table_qam64LowSE,
                                srsran_dci_format_nr_1_0,
                                srsran_search_space_type_ue,
                                srsran_rnti_type_c);
  TESTASSERT_EQ(24, mcs);

  mcs = srsran_ra_nr_cqi_to_mcs(15,
                                SRSRAN_CSI_CQI_TABLE_3,
                                srsran_mcs_table_qam64LowSE,
                                srsran_dci_format_nr_1_0,
                                srsran_search_space_type_ue,
                                srsran_rnti_type_c);
  TESTASSERT_EQ(28, mcs);

  //  TEST CQI Table 1 - MCS table 2
  mcs = srsran_ra_nr_cqi_to_mcs(6,
                                SRSRAN_CSI_CQI_TABLE_1,
                                srsran_mcs_table_256qam,
                                srsran_dci_format_nr_1_1,
                                srsran_search_space_type_ue,
                                srsran_rnti_type_c);
  TESTASSERT_EQ(4, mcs);

  mcs = srsran_ra_nr_cqi_to_mcs(14,
                                SRSRAN_CSI_CQI_TABLE_1,
                                srsran_mcs_table_256qam,
                                srsran_dci_format_nr_1_1,
                                srsran_search_space_type_ue,
                                srsran_rnti_type_c);
  TESTASSERT_EQ(19, mcs);

  //  TEST CQI Table 1 - MCS table 3
  mcs = srsran_ra_nr_cqi_to_mcs(7,
                                SRSRAN_CSI_CQI_TABLE_1,
                                srsran_mcs_table_qam64LowSE,
                                srsran_dci_format_nr_1_0,
                                srsran_search_space_type_ue,
                                srsran_rnti_type_c);
  TESTASSERT_EQ(16, mcs);

  mcs = srsran_ra_nr_cqi_to_mcs(10,
                                SRSRAN_CSI_CQI_TABLE_1,
                                srsran_mcs_table_qam64LowSE,
                                srsran_dci_format_nr_1_0,
                                srsran_search_space_type_ue,
                                srsran_rnti_type_c);
  TESTASSERT_EQ(22, mcs);

  //  TEST CQI Table 2 - MCS table 1
  mcs = srsran_ra_nr_cqi_to_mcs(3,
                                SRSRAN_CSI_CQI_TABLE_2,
                                srsran_mcs_table_64qam,
                                srsran_dci_format_nr_1_0,
                                srsran_search_space_type_ue,
                                srsran_rnti_type_c);
  TESTASSERT_EQ(6, mcs);

  mcs = srsran_ra_nr_cqi_to_mcs(11,
                                SRSRAN_CSI_CQI_TABLE_2,
                                srsran_mcs_table_64qam,
                                srsran_dci_format_nr_1_0,
                                srsran_search_space_type_ue,
                                srsran_rnti_type_c);
  TESTASSERT_EQ(26, mcs);

  //  TEST CQI Table 2 - MCS table 3
  mcs = srsran_ra_nr_cqi_to_mcs(7,
                                SRSRAN_CSI_CQI_TABLE_2,
                                srsran_mcs_table_qam64LowSE,
                                srsran_dci_format_nr_1_0,
                                srsran_search_space_type_ue,
                                srsran_rnti_type_c);
  TESTASSERT_EQ(22, mcs);

  mcs = srsran_ra_nr_cqi_to_mcs(10,
                                SRSRAN_CSI_CQI_TABLE_2,
                                srsran_mcs_table_qam64LowSE,
                                srsran_dci_format_nr_1_0,
                                srsran_search_space_type_ue,
                                srsran_rnti_type_c);
  TESTASSERT_EQ(28, mcs);

  //  TEST CQI Table 3 - MCS table 1
  mcs = srsran_ra_nr_cqi_to_mcs(2,
                                SRSRAN_CSI_CQI_TABLE_3,
                                srsran_mcs_table_64qam,
                                srsran_dci_format_nr_1_0,
                                srsran_search_space_type_ue,
                                srsran_rnti_type_c);
  TESTASSERT_EQ(0, mcs);

  mcs = srsran_ra_nr_cqi_to_mcs(13,
                                SRSRAN_CSI_CQI_TABLE_3,
                                srsran_mcs_table_64qam,
                                srsran_dci_format_nr_1_0,
                                srsran_search_space_type_ue,
                                srsran_rnti_type_c);
  TESTASSERT_EQ(20, mcs);

  //  TEST CQI Table 3 - MCS table 2
  mcs = srsran_ra_nr_cqi_to_mcs(5,
                                SRSRAN_CSI_CQI_TABLE_3,
                                srsran_mcs_table_256qam,
                                srsran_dci_format_nr_1_1,
                                srsran_search_space_type_ue,
                                srsran_rnti_type_c);
  TESTASSERT_EQ(1, mcs);

  mcs = srsran_ra_nr_cqi_to_mcs(14,
                                SRSRAN_CSI_CQI_TABLE_3,
                                srsran_mcs_table_256qam,
                                srsran_dci_format_nr_1_1,
                                srsran_search_space_type_ue,
                                srsran_rnti_type_c);
  TESTASSERT_EQ(15, mcs);
}

int main()
{
  test_cqi_to_mcs();
}