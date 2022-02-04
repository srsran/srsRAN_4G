# Changelog

## [2022.2.4] port_local_changes

### Added

- added function `ieee_float_to_hex` to `cc_worker.cc` and `cc_worker.h` (source: http://www.cplusplus.com/forum/general/63755/)

- added `maker.sh` to streamline rebuilding and installation

### Changed

- modified functions `cc_worker:read_pusch_d` and `cc_worker:read_pucch_d` in `cc_worker.cc` to output hex IQ data to terminal

- modified `CMakeLists.txt` to exclusively rebuild srsenb
