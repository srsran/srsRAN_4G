libLTE
========

libLTE is a free and open-source LTE library for SDR UE and eNodeB. The library does not rely on any external dependencies or frameworks. 

The license is LGPLv3.

The project contains a set of Python tools for the automatic code generation of modules for popular SDR frameworks, including GNURadio, ALOE++, IRIS, and OSSIE. These tools are easy to use and adapt for generating targets for specific platforms or frameworks. 

Download & Install Instructions
=================================

```
git clone https://github.com/ismagom/libLTE.git
cd libLTE
mkdir build
cd build
cmake ../
make 
sudo make install
```

Cell Search Example
====================

This program uses any hardware supported by the UHD driver to scan an LTE band for active cells. See http://niviuk.free.fr/lte_band.php for a list of available bands. The program first obtains a power spectral density of the entire band. For all frequencies with an RSSI higher than a threshold, it tries to find the LTE Primary Synchronization Signal (PSS) and then identifies the CELL ID using the Secondary Synchronization Signal (SSS). Finally, it estimates the Carrier Frequency Offset (CFO) and Sampling Frequency Offset (SFO). 

For instance, the command:

``` pss_scan_usrp -b 3 ```


Scans the LTE band 3 (1805 to 1880 MHz). Note that you need a hardware supporting these frequencies (e.g. SBX daughterboard for USRP). The program outputs the following:

```
Opening UHD device...
-- Opening a USRP2/N-Series device...
-- Current recv frame size: 1472 bytes
-- Current send frame size: 1472 bytes
RSSI scan: 749 freqs in band 3, RSSI threshold -30.00 dBm
Freq 1879.0 Mhz - RSSI: -43.96 dBm
Done. Starting PSS search on 75 channels

UHD Warning:
    The hardware does not support the requested RX sample rate:
    Target sample rate: 1.920000 MSps
    Actual sample rate: 1.923077 MSps
[199/749]: EARFCN 1399 Freq. 1824.90 MHz No PSS found
[200/749]: FOUND EARFCN 1400 Freq. 1825.00 MHz, RSSI -22.43 dBm, PAR 15.86 dB, CFO=-0.25 KHz, SFO=+3.099 KHz, CELL_ID=150
[433/749]: EARFCN 1633 Freq. 1848.30 MHz No PSS found

Done
```

indicating that a Cell with ID 150 has been found at 1825.0 MHz. PAR indicates the peak-to-average ratio (in dB) at the output of the PSS correlator. 

For more command arguments, type ``` pss_scan_usrp --help ```

