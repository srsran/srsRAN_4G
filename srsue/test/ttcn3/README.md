srsUE conformance testing
=========================

This folder includes an srsUE extension that allows to test
the upper-layer protocol stack against the UE conformance tests specified
by 3GPP/ETSI in TS 36.523 [1].

The tester itself is built with Eclipse Titan [2] using the 3GPP EUTRA (LTE/EPC) UE Test Suites [3]. It is, however,
not part of the srsLTE repository.

The extensions present in this folder add a basic system simulator (SS) that talks over an ideal PHY to the 
fully unmodified UE upper-layer stack. It SS itself interfaces over multiple TCP/IP sockets to the TTCN3 tester, i.e.,
implements (a subset of) the interfaces specified in [1].


Resources
---------

[1] https://www.etsi.org/deliver/etsi_ts/136500_136599/13652301

[2] https://projects.eclipse.org/projects/tools.titan

[3] http://www.ttcn-3.org/index.php/downloads/publicts/publicts-3gpp
