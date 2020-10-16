INCLUDE(FindPkgConfig)
#PKG_CHECK_MODULES(UHD uhd)
IF(NOT UHD_FOUND)

FIND_PATH(
    UHD_INCLUDE_DIRS
    NAMES uhd.h
    HINTS $ENV{UHD_DIR}/include
    PATHS /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    UHD_LIBRARIES
    NAMES uhd
    HINTS $ENV{UHD_DIR}/lib
    PATHS /usr/local/lib
          /usr/lib
          /usr/lib/x86_64-linux-gnu
          /usr/local/lib64
          /usr/local/lib32
)

message(STATUS "UHD LIBRARIES " ${UHD_LIBRARIES})
message(STATUS "UHD INCLUDE DIRS " ${UHD_INCLUDE_DIRS})

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(UHD DEFAULT_MSG UHD_LIBRARIES UHD_INCLUDE_DIRS)
MARK_AS_ADVANCED(UHD_LIBRARIES UHD_INCLUDE_DIRS)

include(CheckCXXSourceCompiles)

IF(UHD_FOUND)
  # UHD library directory
  get_filename_component(UHD_LIBRARY_DIR ${UHD_LIBRARIES} DIRECTORY)

  # Save current required variables
  set(_CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS})
  set(_CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES})
  set(_CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES})

  # Set required variables
  set(CMAKE_REQUIRED_INCLUDES ${UHD_INCLUDE_DIRS})
  set(CMAKE_REQUIRED_FLAGS "${CMAKE_CXX_FLAGS} -L${UHD_LIBRARY_DIR}")
  set(CMAKE_REQUIRED_LIBRARIES uhd boost_program_options boost_system)

  # Checks whether the UHD driver supports X300 reset from srsLTE. This functionality requires changing the function
  # `x300_make_ctrl_iface_enet` visibility in the file `uhd/host/lib/usrp/x300_fw_ctrl.cpp`. This can be accomplished
  # adding the following line:
  #   `UHD_API wb_iface::sptr x300_make_ctrl_iface_enet(uhd::transport::udp_simple::sptr udp, bool enable_errors);`
  check_cxx_source_compiles("#include <uhd.h>
    #include <uhd/usrp/multi_usrp.hpp>
    #include <uhd/transport/udp_simple.hpp>

    uhd::wb_iface::sptr x300_make_ctrl_iface_enet(uhd::transport::udp_simple::sptr udp, bool enable_errors);

    uhd_error try_usrp_x300_reset()
    {
      uhd::transport::udp_simple::sptr udp_simple = uhd::transport::udp_simple::make_connected(\"\", \"49152\");
      uhd::wb_iface::sptr x300_ctrl = x300_make_ctrl_iface_enet(udp_simple, true);
      x300_ctrl->poke32(0x100058, 1);
      return UHD_ERROR_NONE;
    }

    int main(int argc, char** argv)
    {
      try_usrp_x300_reset();
      return 0;
    }" UHD_ENABLE_X300_FW_RESET)

  # Checks whether the UHD driver supports X300 custom RF-NOC devices
  check_cxx_source_compiles("#include <uhd.h>
    #include <uhd/device3.hpp>
    #include <uhd/rfnoc/ddc_block_ctrl.hpp>
    #include <uhd/rfnoc/radio_ctrl.hpp>

    static uhd::device3::sptr device3;
    static uhd::rfnoc::ddc_block_ctrl::sptr ddc_ctrl;
    static uhd::rfnoc::radio_ctrl::sptr radio_ctrl;

    uhd_error try_device3_ddc_ctrl()
    {
      ddc_ctrl = device3->get_block_ctrl<uhd::rfnoc::ddc_block_ctrl>(uhd::rfnoc::block_id_t(\"DDC_0\"));
      return UHD_ERROR_NONE;
    }

    uhd_error try_device3_radio_ctrl()
    {
      radio_ctrl = device3->get_block_ctrl<uhd::rfnoc::radio_ctrl>(uhd::rfnoc::block_id_t(\"Radio_0\"));
      return UHD_ERROR_NONE;
    }

    int main(int argc, char** argv)
    {
      try_device3_ddc_ctrl();
      try_device3_radio_ctrl();
      return 0;
    }" UHD_ENABLE_RFNOC)

  # Checks whether the UHD driver supports X300 custom RF-NOC devices
  check_cxx_source_compiles("#include <uhd.h>
    #include <uhd/device3.hpp>
    #include <uhd/rfnoc/ddc_ch2_block_ctrl.hpp>

    static uhd::device3::sptr device3;
    static uhd::rfnoc::ddc_ch2_block_ctrl::sptr ddc_ctrl;

    uhd_error try_device3_ddc_ch2_ctrl()
    {
      ddc_ctrl = device3->get_block_ctrl<uhd::rfnoc::ddc_ch2_block_ctrl>(uhd::rfnoc::block_id_t(\"DDCch2_0\"));
      return UHD_ERROR_NONE;
    }

    int main(int argc, char** argv)
    {
      try_device3_ddc_ch2_ctrl();
      return 0;
    }" UHD_ENABLE_CUSTOM_RFNOC)

  # Recover required variables
  set(CMAKE_REQUIRED_FLAGS ${_CMAKE_REQUIRED_FLAGS})
  set(CMAKE_REQUIRED_LIBRARIES ${_CMAKE_REQUIRED_LIBRARIES})
  set(CMAKE_REQUIRED_INCLUDES ${_CMAKE_REQUIRED_INCLUDES})
ENDIF(UHD_FOUND)

ENDIF(NOT UHD_FOUND)
