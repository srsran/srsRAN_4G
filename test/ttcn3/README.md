# TTCN-3 UE conformance tests

Runs the 3GPP E-UTRA UE conformance test suite ([TS 36.523-3](https://www.3gpp.org/ftp/Specs/archive/36_series/36.523-3/))
against srsUE, with [Eclipse Titan](https://projects.eclipse.org/projects/tools.titan) as tester and srsUE's `ttcn3_dut`
as device under test. The DUT replaces the PHY of srsUE and is connected to the system simulator of the test suite over SCTP.

The test suite itself is downloaded from 3GPP and patched with `ue_testsuite_diff/latest.diff`, and the Titan test
ports are fetched from the upstream Eclipse repositories. `gen_conf_testcases.awk` generates `LTE_EPS_TS_Testcases`
from the test suite, with the E-UTRA test cases only. The srsRAN specific glue code is in `src`. The control
functions of the IPL4 translation ports are generated from the template of the upstream IPL4asp test port and their
test port skeletons by the Titan compiler. The security algorithms are taken from srsRAN's `lib`.

The RRC messages are encoded with Titan's PER codec, which needs Titan 10.1.1 or newer. Distribution packages are too
old (e.g. 8.2.0 on Ubuntu 24.04), so the official [Eclipse Titan release](https://download.eclipse.org/titan/) is used.

## Running in a container

From the repository root (the container needs privileges for SCTP, and the `sctp` kernel module must be loaded on the host):

```
docker build -f test/ttcn3/Dockerfile -t srsue-ttcn3 .
docker run --rm --privileged -v $PWD/ttcn3_logs:/logs srsue-ttcn3
```

This runs all test cases srsUE is expected to pass, listed in `src/srsue_conf_testcases.ttcn`. They are also run for
every pull request and every push to master. Another control module or a single test case can be passed as argument:

```
docker run --rm --privileged -v $PWD/ttcn3_logs:/logs srsue-ttcn3 LTE_EPS_TS_Testcases.TC_9_4_1
```

The logs of Titan and the DUT, and a MAC pcap per test case are written to the mounted log directory.
The DUT is built with AddressSanitizer by default, any ASAN report fails the run. To build without it, add
`--build-arg ENABLE_ASAN=OFF`.

## Running without container

Extract the Eclipse Titan release for your distribution (see `TITAN_RELEASE` in the Dockerfile), install the build
dependencies listed in the Dockerfile, then build the tester:

```
export TTCN3_DIR=/path/to/titan
export PATH=$TTCN3_DIR/bin:$PATH
cd test/ttcn3
./get_testsuite.sh
./get_testports.sh
mkdir build && cd build
../install.sh
```

Build srsRAN with `-DENABLE_TTCN3=ON` and run `run_conformance.sh`, pointing it to the binaries and a log directory:

```
export LD_LIBRARY_PATH=$PWD/test/ttcn3/build:$TTCN3_DIR/lib
TTCN3_DUT=$PWD/build/srsue/src/test/ttcn3/src/ttcn3_dut TTCN3_TESTER_DIR=$PWD/test/ttcn3/build \
  TTCN3_LOG_DIR=$PWD/test/ttcn3/logs test/ttcn3/run_conformance.sh srsue_conf_testcases
```

Titan writes its logs to `../logs` relative to the tester build directory, so `TTCN3_LOG_DIR` should be
`test/ttcn3/logs` in this case.
