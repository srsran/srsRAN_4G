/**
 * Copyright 2013-2023 Software Radio Systems Limited
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

#include "srsgnb/hdr/stack/ric/e2sm_kpm_report_service.h"

e2sm_kpm_report_service::e2sm_kpm_report_service(e2sm_kpm*                     e2sm_kpm,
                                                 uint16_t                      action_id,
                                                 e2_sm_kpm_action_definition_s action_definition) :
  parent(e2sm_kpm),
  action_id(action_id),
  action_def_generic(action_definition),
  ric_ind_header_generic(),
  ric_ind_header(ric_ind_header_generic.ind_hdr_formats.ind_hdr_format1()),
  meas_collection_timer(parent->task_sched_ptr->get_unique_timer())
{
}

std::vector<e2sm_kpm_label_enum>
e2sm_kpm_report_service::_get_present_labels(const meas_info_item_s& action_meas_info_item)
{
  std::vector<e2sm_kpm_label_enum> labels;
  // TODO: add all labels defined in e2sm_kpm doc
  for (uint32_t l = 0; l < action_meas_info_item.label_info_list.size(); l++) {
    if (action_meas_info_item.label_info_list[l].meas_label.no_label_present) {
      labels.push_back(NO_LABEL);
    }
    if (action_meas_info_item.label_info_list[l].meas_label.min_present) {
      labels.push_back(MIN_LABEL);
    }
    if (action_meas_info_item.label_info_list[l].meas_label.max_present) {
      labels.push_back(MAX_LABEL);
    }
    if (action_meas_info_item.label_info_list[l].meas_label.avg_present) {
      labels.push_back(AVG_LABEL);
    }
    if (action_meas_info_item.label_info_list[l].meas_label.sum_present) {
      labels.push_back(SUM_LABEL);
    }
  }
  return labels;
}

bool e2sm_kpm_report_service::_initialize_ric_ind_hdr()
{
  // TODO: set the remaining fields properly (they are optional)
  ric_ind_header.collet_start_time.from_number(std::time(0));
  // ric_ind_header.file_formatversion.from_string(hdr.file_formatversion);
  // ric_ind_header.sender_name.from_string(hdr.sender_name);
  // ric_ind_header.sender_type.from_string(hdr.sender_type);
  // ric_ind_header.vendor_name.from_string(hdr.vendor_name);
  return true;
}

meas_record_item_c::types e2sm_kpm_report_service::_get_meas_data_type(std::string         meas_name,
                                                                       e2sm_kpm_label_enum label,
                                                                       meas_record_l&      meas_record_list)
{
  meas_record_item_c::types data_type = meas_record_item_c::types::options::nulltype;
  // if no data collected check the type using metric definition
  if (meas_record_list.size() == 0) {
    e2sm_kpm_metric_t metric_definition;
    if (not parent->_get_meas_definition(meas_name, metric_definition)) {
      parent->logger.debug("No definition for measurement type \"%s\"", metric_definition.name);
      return data_type;
    }
    if (metric_definition.data_type == INTEGER) {
      data_type = meas_record_item_c::types::options::integer;
    } else {
      data_type = meas_record_item_c::types::options::real;
    }
  } else {
    // check the data type of the first element in the list
    data_type = meas_record_list[0].type();
  }
  return data_type;
}

bool e2sm_kpm_report_service::_start_meas_collection()
{
  if (granul_period) {
    printf("Start collecting measurements every %i ms\n", granul_period);
    parent->logger.debug("Start collecting measurements every every %i ms", granul_period);
    meas_collection_timer.set(granul_period, [this](uint32_t tid) { this->_collect_meas_data(); });
    meas_collection_timer.run();
    return true;
  }
  return false;
}

bool e2sm_kpm_report_service::stop()
{
  return _stop_meas_collection();
}

bool e2sm_kpm_report_service::_stop_meas_collection()
{
  if (meas_collection_timer.is_running()) {
    printf("Stop collecting measurements every %i ms\n", granul_period);
    parent->logger.debug("Stop collecting measurements every %i ms\n", granul_period);
    meas_collection_timer.stop();
    return true;
  }
  return false;
}

bool e2sm_kpm_report_service::_reschedule_meas_collection()
{
  if (granul_period) {
    meas_collection_timer.run();
    return true;
  }
  return false;
}

e2sm_kpm_report_service_style1::e2sm_kpm_report_service_style1(e2sm_kpm*                     e2sm_kpm,
                                                               uint16_t                      action_id,
                                                               e2_sm_kpm_action_definition_s action_definition) :
  e2sm_kpm_report_service(e2sm_kpm, action_id, action_definition),
  action_def(action_def_generic.action_definition_formats.action_definition_format1()),
  ric_ind_message(ric_ind_message_generic.ind_msg_formats.set_ind_msg_format1())
{
  ind_msg_format         = e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::types_opts::ind_msg_format1;
  granul_period          = action_def.granul_period;
  cell_global_id_present = action_def.cell_global_id_present;
  if (cell_global_id_present) {
    cell_global_id = action_def.cell_global_id;
  }

  this->_initialize_ric_ind_hdr();
  this->_initialize_ric_ind_msg();

  granul_period = 1000; // TODO: overwrite for testing
  _start_meas_collection();
}

bool e2sm_kpm_report_service_style1::_initialize_ric_ind_msg()
{
  meas_info_list_l action_meas_info_list = action_def.meas_info_list;

  // ric_ind_message.granul_period_present = true;
  // ric_ind_message.granul_period = granul_period; // TODO: our asn1 has some issues with this field
  ric_ind_message.granul_period = 0;
  ric_ind_message.meas_info_list.resize(action_meas_info_list.size());
  ric_ind_message.meas_data.resize(action_meas_info_list.size());

  // add measurement info
  for (uint32_t i = 0; i < ric_ind_message.meas_info_list.size(); i++) {
    // structs to fill
    meas_info_item_s& meas_info_item = ric_ind_message.meas_info_list[i];

    // measurements definition
    meas_info_item_s& meas_def_item = action_meas_info_list[i];
    std::string       meas_name     = meas_def_item.meas_type.meas_name().to_string();

    meas_info_item.meas_type.set_meas_name().from_string(meas_name.c_str());
    meas_info_item.label_info_list.resize(meas_def_item.label_info_list.size());

    // TODO: add all labels defined in e2sm_kpm doc, make this part generic and put to the base class
    for (uint32_t l = 0; l < meas_def_item.label_info_list.size(); l++) {
      if (meas_def_item.label_info_list[l].meas_label.no_label_present) {
        meas_info_item.label_info_list[l].meas_label.no_label_present = true;
        meas_info_item.label_info_list[l].meas_label.no_label         = meas_label_s::no_label_opts::true_value;
      }
      if (meas_def_item.label_info_list[l].meas_label.min_present) {
        meas_info_item.label_info_list[l].meas_label.min_present = true;
        meas_info_item.label_info_list[l].meas_label.min         = meas_label_s::min_opts::true_value;
      }
      if (meas_def_item.label_info_list[l].meas_label.max_present) {
        meas_info_item.label_info_list[l].meas_label.max_present = true;
        meas_info_item.label_info_list[l].meas_label.max         = meas_label_s::max_opts::true_value;
      }
      if (meas_def_item.label_info_list[l].meas_label.avg_present) {
        meas_info_item.label_info_list[l].meas_label.avg_present = true;
        meas_info_item.label_info_list[l].meas_label.avg         = meas_label_s::avg_opts::true_value;
      }
      if (meas_def_item.label_info_list[l].meas_label.sum_present) {
        meas_info_item.label_info_list[l].meas_label.sum_present = true;
        meas_info_item.label_info_list[l].meas_label.sum         = meas_label_s::sum_opts::true_value;
      }
    }
  }

  return true;
}

bool e2sm_kpm_report_service_style1::process_ric_action_definition(e2sm_kpm*                      e2sm_kpm,
                                                                   e2_sm_kpm_action_definition_s& action_def_generic)
{
  e2_sm_kpm_action_definition_format1_s& action_definition =
      action_def_generic.action_definition_formats.action_definition_format1();

  bool             cell_global_id_present = false;
  uint64_t         granul_period;
  uint64_t         eutra_cell_id;
  uint64_t         plmn_id;
  ueid_c           ue_id;
  meas_info_list_l meas_info_list;

  granul_period = action_definition.granul_period;

  if (granul_period == 0) {
    e2sm_kpm->logger.debug("Action granularity period of %i is not supported -> do not admitted action\n",
                           granul_period);
    return false;
  }

  if (action_definition.cell_global_id_present) {
    cell_global_id_present = true;
    if (action_definition.cell_global_id.type() == cgi_c::types_opts::eutra_cgi) {
      eutra_cell_id = action_definition.cell_global_id.eutra_cgi().eutra_cell_id.to_number();
      plmn_id       = action_definition.cell_global_id.eutra_cgi().plmn_id.to_number();
      e2sm_kpm->logger.debug("plmn_id 0x%x, eutra_cell_id %i", plmn_id, eutra_cell_id);
      //  TODO: check if E2 node has cell_id and plmn_id
    }
  }

  std::map<std::string, e2sm_kpm_label_enum> admitted_metrics;
  meas_info_list = action_definition.meas_info_list;
  for (uint32_t i = 0; i < meas_info_list.size(); i++) {
    std::string       meas_name = meas_info_list[i].meas_type.meas_name().to_string();
    e2sm_kpm_metric_t metric_definition;
    if (not e2sm_kpm->_get_meas_definition(meas_name, metric_definition)) {
      printf("Unsupported measurement name: \"%s\" --> do not admit action\n", meas_name.c_str());
      return false;
    }

    uint32_t nof_labels = 0;
    // TODO: add all labels defined in e2sm_kpm doc, make this part generic and put to base class
    // TODO: check if metric is supported at the requested level, i.e., CELL_LEVEL if cell_global_id_present == true,
    // and ENB_LEVEL otherwise
    for (uint32_t l = 0; l < meas_info_list[i].label_info_list.size(); l++) {
      if (meas_info_list[i].label_info_list[l].meas_label.no_label_present) {
        if (metric_definition.supported_labels & NO_LABEL) {
          nof_labels++;
          admitted_metrics[meas_name] = NO_LABEL;
        } else {
          printf("Unsupported label: NO_LABEL for metric \"%s\" --> do not admit action\n", meas_name.c_str());
          return false;
        }
      }
      if (meas_info_list[i].label_info_list[l].meas_label.min_present) {
        if (metric_definition.supported_labels & MIN_LABEL) {
          nof_labels++;
          admitted_metrics[meas_name] = MIN_LABEL;
        } else {
          printf("Unsupported label: MIN_LABEL for metric \"%s\" --> do not admit action\n", meas_name.c_str());
          return false;
        }
      }
      if (meas_info_list[i].label_info_list[l].meas_label.max_present) {
        if (metric_definition.supported_labels & MAX_LABEL) {
          nof_labels++;
          admitted_metrics[meas_name] = MAX_LABEL;
        } else {
          printf("Unsupported label: MAX_LABEL for metric \"%s\" --> do not admit action\n", meas_name.c_str());
          return false;
        }
      }
      if (meas_info_list[i].label_info_list[l].meas_label.avg_present) {
        if (metric_definition.supported_labels & AVG_LABEL) {
          nof_labels++;
          admitted_metrics[meas_name] = AVG_LABEL;
        } else {
          printf("Unsupported label: AVG_LABEL for metric \"%s\" --> do not admit action\n", meas_name.c_str());
          return false;
        }
      }
    }

    // Note: currently we use labels as choice (i.e., only one can be present) as documentation is not clear about it
    if (nof_labels > 1) {
      printf("Only one label per metric can be present, meas: \"%s\" has %i labels --> do not admit action\n",
             meas_name.c_str(),
             nof_labels);
      return false;
    }
  }

  printf("Admitted action with the following metrics and labels: \n");
  for (const auto& it : admitted_metrics) {
    std::string meas_name = it.first;
    std::string label_str = e2sm_kpm_label_2_str(it.second);
    printf("--- Metric: \"%s\" with label: %s\n", meas_name.c_str(), label_str.c_str());
  }

  return true;
}

meas_data_item_s& e2sm_kpm_report_service_style1::_get_meas_data_item(std::string         meas_name,
                                                                      e2sm_kpm_label_enum label,
                                                                      uint32_t            ue_id,
                                                                      bool&               ref_found)
{
  meas_info_list_l& meas_info_list = ric_ind_message.meas_info_list;
  ref_found                        = false;
  // find proper index of the metric
  for (uint32_t i = 0; i < meas_info_list.size(); i++) {
    // measurements definition
    meas_info_item_s meas_def_item = meas_info_list[i];
    std::string      meas_def_name = meas_def_item.meas_type.meas_name().to_string();

    // check if the metric name matches
    if (meas_def_name != meas_name.c_str()) {
      continue;
    }

    // check if the metric label matches
    // TODO: add all labels defined in e2sm_kpm doc
    for (uint32_t l = 0; l < meas_def_item.label_info_list.size(); l++) {
      if (meas_def_item.label_info_list[l].meas_label.no_label_present and label == NO_LABEL) {
        ref_found = true;
        return ric_ind_message.meas_data[i];
      }
      if (meas_def_item.label_info_list[l].meas_label.min_present and label == MIN_LABEL) {
        ref_found = true;
        return ric_ind_message.meas_data[i];
      }
      if (meas_def_item.label_info_list[l].meas_label.max_present and label == MAX_LABEL) {
        ref_found = true;
        return ric_ind_message.meas_data[i];
      }
      if (meas_def_item.label_info_list[l].meas_label.avg_present and label == AVG_LABEL) {
        ref_found = true;
        return ric_ind_message.meas_data[i];
      }
      if (meas_def_item.label_info_list[l].meas_label.sum_present and label == SUM_LABEL) {
        ref_found = true;
        return ric_ind_message.meas_data[i];
      }
    }
  }
  // TODO assert if match == false, has to be present as was created during initialization
  ref_found = false;
  return ric_ind_message.meas_data[0];
}

bool e2sm_kpm_report_service_style1::_collect_meas_data()
{
  meas_info_list_l& meas_info_list = ric_ind_message.meas_info_list;
  for (uint32_t i = 0; i < meas_info_list.size(); i++) {
    meas_info_item_s&                meas_def_item = meas_info_list[i];
    std::string                      meas_name     = meas_def_item.meas_type.meas_name().to_string();
    std::vector<e2sm_kpm_label_enum> labels        = _get_present_labels(meas_def_item);

    for (const auto& label : labels) {
      // TODO: probably some labels need a special processing (e.g., use bin width that needs to be stored)
      // get a proper record list
      bool              ref_found      = false;
      meas_data_item_s& meas_data_item = _get_meas_data_item(meas_name, label, 0, ref_found);
      if (not ref_found) {
        parent->logger.info("Cannot find a meas record list, action_id %i, metric \"%s\" label: %i",
                            action_id,
                            meas_name.c_str(),
                            label);
        return false;
      }

      // get data type
      meas_record_item_c::types data_type = _get_meas_data_type(meas_name, label, meas_data_item.meas_record);

      // extract a needed value from enb metrics and add to the proper meas record list
      e2sm_kpm_meas_def_t meas_value;
      meas_value.name  = meas_name;
      meas_value.label = label;
      meas_value.scope = ENB_LEVEL;
      if (cell_global_id_present) {
        meas_value.scope   = CELL_LEVEL;
        meas_value.cell_id = 0;
      }
      meas_value.data_type = data_type;

      meas_record_item_c item;
      if (not parent->_collect_meas_value(meas_value, item)) {
        parent->logger.info("Cannot extract value \"%s\" label: %i", meas_name.c_str(), label);
        return false;
      }

      // save meas value in the proper record list
      meas_data_item.meas_record.push_back(item);
    }
  }

  // reschedule measurement collection
  _reschedule_meas_collection();
  return true;
}

bool e2sm_kpm_report_service_style1::is_ric_ind_ready()
{
  // TODO: check if only NO_VALUES, if so then skip
  return true;
}

bool e2sm_kpm_report_service_style1::clear_collected_data()
{
  ric_ind_header.collet_start_time.from_number(std::time(0));
  for (uint32_t i = 0; i < ric_ind_message.meas_data.size(); ++i) {
    ric_ind_message.meas_data[i].meas_record.clear();
  }
  return true;
}

e2sm_kpm_report_service_style2::e2sm_kpm_report_service_style2(e2sm_kpm*                     e2sm_kpm,
                                                               uint16_t                      action_id,
                                                               e2_sm_kpm_action_definition_s action_definition) :
  e2sm_kpm_report_service(e2sm_kpm, action_id, action_definition),
  action_def(action_def_generic.action_definition_formats.action_definition_format2()),
  ric_ind_message(ric_ind_message_generic.ind_msg_formats.set_ind_msg_format1())
{
  ind_msg_format = e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::types_opts::ind_msg_format1;
  this->_initialize_ric_ind_hdr();
  this->_initialize_ric_ind_msg();
}

bool e2sm_kpm_report_service_style2::_initialize_ric_ind_msg()
{
  // TODO: implement
  return false;
}

bool e2sm_kpm_report_service_style2::process_ric_action_definition(e2sm_kpm*                      e2sm_kpm,
                                                                   e2_sm_kpm_action_definition_s& action_def_generic)
{
  // TODO: implement
  // note: similar to e2sm_kpm_report_service_style1::process_ric_action_definition but in addition
  // we need to check whether measurement is supported at UE_LEVEL
  return false;
}

bool e2sm_kpm_report_service_style2::_collect_meas_data()
{
  // TODO: implement
  return false;
}

bool e2sm_kpm_report_service_style2::is_ric_ind_ready()
{
  // TODO: check if only NO_VALUES, if so then skip
  return false;
}

bool e2sm_kpm_report_service_style2::clear_collected_data()
{
  // TODO: implement
  return false;
}

e2sm_kpm_report_service_style3::e2sm_kpm_report_service_style3(e2sm_kpm*                     e2sm_kpm,
                                                               uint16_t                      action_id,
                                                               e2_sm_kpm_action_definition_s action_definition) :
  e2sm_kpm_report_service(e2sm_kpm, action_id, action_definition),
  action_def(action_def_generic.action_definition_formats.action_definition_format3()),
  ric_ind_message(ric_ind_message_generic.ind_msg_formats.set_ind_msg_format2())
{
  ind_msg_format = e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::types_opts::ind_msg_format1;
  this->_initialize_ric_ind_hdr();
  this->_initialize_ric_ind_msg();
}

bool e2sm_kpm_report_service_style3::_initialize_ric_ind_msg()
{
  // TODO: implement
  return false;
}

bool e2sm_kpm_report_service_style3::process_ric_action_definition(e2sm_kpm*                      e2sm_kpm,
                                                                   e2_sm_kpm_action_definition_s& action_def_generic)
{
  // TODO: implement
  return false;
}

bool e2sm_kpm_report_service_style3::_collect_meas_data()
{
  // TODO: implement
  return false;
}

bool e2sm_kpm_report_service_style3::is_ric_ind_ready()
{
  // TODO: check if only NO_VALUES, if so then skip
  return false;
}

bool e2sm_kpm_report_service_style3::clear_collected_data()
{
  // TODO: implement
  return false;
}

e2sm_kpm_report_service_style4::e2sm_kpm_report_service_style4(e2sm_kpm*                     e2sm_kpm,
                                                               uint16_t                      action_id,
                                                               e2_sm_kpm_action_definition_s action_definition) :
  e2sm_kpm_report_service(e2sm_kpm, action_id, action_definition),
  action_def(action_def_generic.action_definition_formats.action_definition_format4()),
  ric_ind_message(ric_ind_message_generic.ind_msg_formats.set_ind_msg_format3())
{
  ind_msg_format = e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::types_opts::ind_msg_format1;
  this->_initialize_ric_ind_hdr();
  this->_initialize_ric_ind_msg();
}

bool e2sm_kpm_report_service_style4::_initialize_ric_ind_msg()
{
  // TODO: implement
  return false;
}

bool e2sm_kpm_report_service_style4::process_ric_action_definition(e2sm_kpm*                      e2sm_kpm,
                                                                   e2_sm_kpm_action_definition_s& action_def_generic)
{
  // TODO: implement
  return false;
}

bool e2sm_kpm_report_service_style4::_collect_meas_data()
{
  // TODO: implement
  return false;
}

bool e2sm_kpm_report_service_style4::is_ric_ind_ready()
{
  // TODO: check if only NO_VALUES, if so then skip
  return false;
}

bool e2sm_kpm_report_service_style4::clear_collected_data()
{
  // TODO: implement
  return false;
}

e2sm_kpm_report_service_style5::e2sm_kpm_report_service_style5(e2sm_kpm*                     e2sm_kpm,
                                                               uint16_t                      action_id,
                                                               e2_sm_kpm_action_definition_s action_definition) :
  e2sm_kpm_report_service(e2sm_kpm, action_id, action_definition),
  action_def(action_def_generic.action_definition_formats.action_definition_format5()),
  ric_ind_message(ric_ind_message_generic.ind_msg_formats.set_ind_msg_format3())
{
  ind_msg_format = e2_sm_kpm_ind_msg_s::ind_msg_formats_c_::types_opts::ind_msg_format1;
  this->_initialize_ric_ind_hdr();
  this->_initialize_ric_ind_msg();
}

bool e2sm_kpm_report_service_style5::_initialize_ric_ind_msg()
{
  // TODO: implement
  return false;
}

bool e2sm_kpm_report_service_style5::process_ric_action_definition(e2sm_kpm*                      e2sm_kpm,
                                                                   e2_sm_kpm_action_definition_s& action_def_generic)
{
  // TODO: implement
  return false;
}

bool e2sm_kpm_report_service_style5::_collect_meas_data()
{
  // TODO: implement
  return false;
}

bool e2sm_kpm_report_service_style5::is_ric_ind_ready()
{
  // TODO: check if only NO_VALUES, if so then skip
  return false;
}

bool e2sm_kpm_report_service_style5::clear_collected_data()
{
  // TODO: implement
  return false;
}