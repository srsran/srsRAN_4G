srsLTE
========

[![Coverity Scan Build Status](https://scan.coverity.com/projects/10045/badge.svg)](https://scan.coverity.com/projects/10045)

srsLTE is a free and open-source LTE software suite developed by SRS (www.softwareradiosystems.com). 

It includes:
  * srsUE - a complete SDR LTE UE application featuring all layers from PHY to IP
  * srsENB - a complete SDR LTE eNodeB application 
  * a highly modular set of common libraries for PHY, MAC, RLC, PDCP, RRC, NAS, S1AP and GW layers. 

srsLTE is released under the AGPLv3 license and uses software from the OpenLTE project (http://sourceforge.net/projects/openlte) for some security functions and for RRC/NAS message parsing.

Common Features
---------------

 * LTE Release 8 compliant (with selected features of Release 9)
 * FDD configuration
 * Tested bandwidths: 1.4, 3, 5, 10, 15 and 20 MHz
 * Transmission mode 1 (single antenna), 2 (transmit diversity), 3 (CCD) and 4 (closed-loop spatial multiplexing)
 * Frequency-based ZF and MMSE equalizer
 * Evolved multimedia broadcast and multicast service (eMBMS)
 * Highly optimized Turbo Decoder available in Intel SSE4.1/AVX2 (+100 Mbps) and standard C (+25 Mbps)
 * MAC, RLC, PDCP, RRC, NAS, S1AP and GW layers
 * Detailed log system with per-layer log levels and hex dumps
 * MAC layer wireshark packet capture
 * Command-line trace metrics
 * Detailed input configuration files

srsUE Features
--------------
 
 * Cell search and synchronization procedure for the UE
 * Soft USIM supporting Milenage and XOR authentication 
 * Virtual network interface *tun_srsue* created upon network attach
 * +100 Mbps DL in 20 MHz MIMO TM4 configuration in i7 Quad-Core CPU.
 * 75 Mbps DL in 20 MHz SISO configuration in i7 Quad-Core CPU.
 * 36 Mbps DL in 10 MHz SISO configuration in i5 Dual-Core CPU.

srsUE has been fully tested and validated with the following network equipment: 
 * Amarisoft LTE100 eNodeB and EPC
 * Nokia FlexiRadio family FSMF system module with 1800MHz FHED radio module and TravelHawk EPC simulator
 * Huawei DBS3900 
 * Octasic Flexicell LTE-FDD NIB

srsENB Features
---------------

 * Round Robin MAC scheduler with FAPI-like C++ API
 * SR support
 * Periodic and Aperiodic CQI feedback support
 * Standard S1AP and GTP-U interfaces to the Core Network
 * Tested up to 75 Mbps DL in SISO configuration with commercial UEs 

srsENB has been tested and validated with the following handsets:
 * LG Nexus 5
 * LG Nexus 4
 * Motorola Moto G4 plus
 * Huawei P9/P9lite
 * Huawei dongles: E3276 and E398

Hardware
--------

The library currently supports the Ettus Universal Hardware Driver (UHD) and the bladeRF driver. Thus, any hardware supported by UHD or bladeRF can be used. There is no sampling rate conversion, therefore the hardware should support 30.72 MHz clock in order to work correctly with LTE sampling frequencies and decode signals from live LTE base stations. 

We have tested the following hardware: 
 * USRP B210
 * USRP X300
 * bladeRF
 * limeSDR (currently, only the PHY-layer examples, i.e., pdsch_enodeb/ue are supported)

Build Instructions
------------------

* Mandatory requirements: 
  * Common:
    * cmake              https://cmake.org/
    * libfftw            http://www.fftw.org/
    * PolarSSL/mbedTLS   https://tls.mbed.org
  * srsUE:
    * Boost:             http://www.boost.org
  * srsENB:
    * Boost:             http://www.boost.org
    * lksctp:            http://lksctp.sourceforge.net/
    * config:            http://www.hyperrealm.com/libconfig/

For example, on Ubuntu 17.04, one can install the required libraries with:
```
sudo apt-get install cmake libfftw3-dev libmbedtls-dev libboost-all-dev libconfig++-dev libsctp-dev
```
Note that depending on your flavor and version of Linux, the actual package names may be different.

* Optional requirements: 
  * srsgui:              https://github.com/srslte/srsgui - for real-time plotting.
  * VOLK:                https://github.com/gnuradio/volk -  if the VOLK library and headers are detected, they will be used to accelerate some signal processing functions.

* RF front-end driver:
  * UHD:                 https://github.com/EttusResearch/uhd
  * BladeRF:             https://github.com/Nuand/bladeRF

Download and build srsLTE: 
```
git clone https://github.com/srsLTE/srsLTE.git
cd srsLTE
mkdir build
cd build
cmake ../
make 
```

The software suite can also be installed using the command ```sudo make install```. 

Execution Instructions
----------------------

The srsUE and srsENB applications include example configuration files. Execute the applications with root privileges to enable real-time thread priorities and to permit creation of virtual network interfaces.

### srsUE

Run the srsUE application as follows:
```
sudo ./srsue ue.conf
```

### srsENB

As the srsLTE software suite does not include EPC functionality, a separate EPC is required to run srsENB. Run the application as follows:
```
sudo ./srsenb enb.conf
```


Support
========

Mailing list: http://www.softwareradiosystems.com/mailman/listinfo/srslte-users
