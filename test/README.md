End-to-End LTE Tests
====================

This folder contains a basic shell script that allows a quick end-to-end LTE test
using srsUE, srsENB, and srsEPC. The script has minimal requirements and should
only take less then 30 seconds for each configuration. Currently the script
only allows to configure the cell bandwidth and the number of component carrier in the eNB.

The script launches all components and puts the UE into a different network namespace
in order to allow IP connectivity to srsEPC on a single machine. It therefore
requires root privelages at the moment.

It then launches, one after another, uplink and downlink ping as well as a quick UDP
downlink session. Between those experiments, the UE stays connected to the eNB/EPC
the entire time.

After the experiments are finished, all components are torn down and the log files
are analyzed.


Single Execution
----------------

The script can be executed manually. It expects the srsLTE build directory as
the first argument, the cell bandwidth as the second, and, optionally the number
of component carrier as the third.

```
$ sudo ./run_lte.sh ~/src/srsLTE/build 50 1
```

Testing all Bandwidths
----------------------

You can also test all bandwidths by letting CTest iterate over all possible PRB sizes.

```
$ cmake -DENABLE_ZMQ_TEST=True ..
$ cd test
$ sudo make test
```

Known Issues
------------

If you've started the script and it failed it may left the network namespace open and thus
the next execution of the script will fail. In this case, just delete the network namespace
before running again:

```
$ sudo ip netns delete ue1
```
