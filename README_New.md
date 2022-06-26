# srsRAN_New

This program is developed based on the srsRAN 22.04 with the following improvements:
- Support for establishing default EPS and IMS EPS simultaneously.
- Support for IPv6 auto-configuration.
- Automatically adds a route to P-CSCF and saves one of P-CSCF addresses to `/tmp/.pcscf`.
- Support for saving user plane traffic to pcap file.

## Configure IMS EPS
Cofnigure the `APN` and `interface` of IMS EPS in NAS and GW sections.
```
#####################################################################
# NAS configuration
...
# ims_enable:        Whether enable IMS APN for VoLTE service
# ims_apn:           Set IMS APN. Default: IMS
# ims_apn_protocol:  Set IMS APN protocol (IPv4, IPv6 or IPv4v6). Default: IPv4v6
#####################################################################
[nas]
...
#ims_enable = false
#ims_apn = ims
#ims_apn_protocol = ipv4v6

#####################################################################
# GW configuration
...
# ims_ip_devname:       Name of the tun_srsue_ims device. Default: tun_srsue_ims
#####################################################################
[gw]
...
#ims_ip_devname = tun_srsue_ims
```

## Saving user plane taffic
Add an entry for DLT=154 with Protocol=udp to DLT_USER.
```
#####################################################################
# Packet capture configuration
...
# enable:            Enable packet captures of layers (mac/mac_nr/nas/gw/none) multiple option list
...
# gw_filename:       File path to use for User Plan packet capture
#####################################################################
[pcap]
enable = none
...
gw_filename = /tmp/ue_gw.pcap
```