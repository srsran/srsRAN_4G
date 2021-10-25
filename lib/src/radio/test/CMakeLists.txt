#
# Copyright 2013-2021 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

if(RF_FOUND)
  add_executable(benchmark_radio benchmark_radio.cc)
  target_link_libraries(benchmark_radio srsran_common srsran_phy srsran_radio)
  if(SRSGUI_FOUND)
    include_directories(${SRSGUI_INCLUDE_DIRS})
    target_link_libraries(benchmark_radio ${SRSGUI_LIBRARIES})
  endif(SRSGUI_FOUND)
  if (ZEROMQ_FOUND)
    add_test(benchmark_radio_multi_rf benchmark_radio -d zmq -a
            tx_port=tcp://*:2000,rx_port=tcp://localhost:2000\;tx_port=tcp://*:2001,rx_port=tcp://localhost:2001\;tx_port=tcp://*:2002,rx_port=tcp://localhost:2002\;tx_port=tcp://*:2003,rx_port=tcp://localhost:2003\;
            -p 4)
  endif (ZEROMQ_FOUND)

  add_executable(test_radio_rt_gain test_radio_rt_gain.cc)
  target_link_libraries(test_radio_rt_gain
          srsran_common
          srsran_phy
          srsran_radio
          ${CMAKE_THREAD_LIBS_INIT}
          ${Boost_LIBRARIES})
  if (ZEROMQ_FOUND)
    add_test(test_radio_rt_gain_zmq test_radio_rt_gain --srate=3.84e6 --dev_name=zmq --dev_args=tx_port=ipc:///tmp/test_radio_rt_gain_zmq,rx_port=ipc:///tmp/test_radio_rt_gain_zmq,base_srate=3.84e6)
  endif (ZEROMQ_FOUND)

endif(RF_FOUND)


