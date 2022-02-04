srsRAN
======

srsRAN is a 4G/5G software radio suite developed by [SRS](http://www.srs.io).

See the [srsRAN project pages](https://www.srsran.com) for information, guides and project news.

The srsRAN suite includes:
  * srsUE - a full-stack SDR 4G/5G-NSA UE application (5G-SA coming soon)
  * srsENB - a full-stack SDR 4G/5G-NSA eNodeB application (5G-SA coming soon)
  * srsEPC - a light-weight 4G core network implementation with MME, HSS and S/P-GW

For application features, build instructions and user guides see the [srsRAN documentation](https://docs.srsran.com).

For license details, see LICENSE file.

Team 13
=======

Team 13 is a group of Boston University Students modifying and using the srsRAN library to model and study C-V2X traffic.

Building & Installation
=======

This repo is configured for ease of rebuilding on machines that currently have srsRAN built and installed.

In order to build this library on a new machine, change the following lines in `CMakeLists.txt` from ""OFF" to "ON":

```
option(ENABLE_SRSUE          "Build srsUE application"                  OFF)
option(ENABLE_SRSUE          "Build srsUE application"                  OFF)
option(ENABLE_BLADERF        "Enable BladeRF"                           OFF)
option(ENABLE_SOAPYSDR       "Enable SoapySDR"                          OFF)
option(ENABLE_SKIQ           "Enable Sidekiq SDK"                       OFF)
```
