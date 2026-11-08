# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: AGPL-3.0-or-later

#
# Container for the 3GPP TTCN-3 UE conformance tests (TS 36.523-3) with Eclipse Titan as tester and srsUE's
# ttcn3_dut as device under test. Build from the repository root and run (needs privileges for SCTP):
#   docker build -f test/ttcn3/Dockerfile -t srsue-ttcn3 .
#   docker run --rm --privileged -v $PWD/ttcn3_logs:/logs srsue-ttcn3
#
# By default the test cases of srsue_conf_testcases are run, another control module or a single test case can be
# passed as argument, e.g.:
#   docker run --rm --privileged -v $PWD/ttcn3_logs:/logs srsue-ttcn3 LTE_EPS_TS_Testcases.TC_9_4_1
#
# The DUT is built with AddressSanitizer by default, to build without it use --build-arg ENABLE_ASAN=OFF
#

ARG OS_VERSION=24.04
ARG ENABLE_ASAN=ON

# Titan tester, only depends on test/ttcn3 so it stays cached when the DUT changes
FROM ubuntu:${OS_VERSION} AS tester

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
      g++ make unzip patch curl git ca-certificates \
      libmbedtls-dev libsctp-dev libssl-dev libxml2-dev \
    && rm -rf /var/lib/apt/lists/*

# Official Eclipse Titan release, the distribution packages are too old for the PER codec of the RRC messages
ARG TITAN_RELEASE=ttcn3-11.2.1-linux64-gcc13-ubuntu_24.04.4_lts_foss.tgz
ARG TITAN_SHA256=6f6d8557313f03c0ea1406eead9dbbdedf640b340d299d37c75b37550623718c
ENV TTCN3_DIR=/opt/titan
ENV PATH=${TTCN3_DIR}/bin:${PATH}
RUN curl -fsSL -o /tmp/titan.tgz https://download.eclipse.org/titan/${TITAN_RELEASE} \
    && echo "${TITAN_SHA256}  /tmp/titan.tgz" | sha256sum -c - \
    && mkdir -p ${TTCN3_DIR} && tar xzf /tmp/titan.tgz --no-same-owner -C ${TTCN3_DIR} \
    && rm -rf /tmp/titan.tgz ${TTCN3_DIR}/demo ${TTCN3_DIR}/doc ${TTCN3_DIR}/help ${TTCN3_DIR}/man

# Keep the repository layout, the tester also builds the security algorithms of srsRAN
WORKDIR /srsran/test/ttcn3
# Only copy what is needed to build, so changes to the run script or test configuration don't trigger a rebuild
COPY test/ttcn3/get_testsuite.sh test/ttcn3/get_testports.sh test/ttcn3/gen_conf_testcases.awk ./
COPY test/ttcn3/ue_testsuite_diff ue_testsuite_diff
RUN ./get_testsuite.sh && ./get_testports.sh
COPY lib/include/srsran/asn1/liblte_common.h /srsran/lib/include/srsran/asn1/
COPY lib/include/srsran/common/liblte_security.h lib/include/srsran/common/s3g.h lib/include/srsran/common/ssl.h \
     lib/include/srsran/common/zuc.h /srsran/lib/include/srsran/common/
COPY lib/src/common/liblte_security.cc lib/src/common/s3g.cc lib/src/common/zuc.cc /srsran/lib/src/common/
COPY test/ttcn3/gen_links.sh test/ttcn3/gen_ipl4_ports.sh test/ttcn3/regen-makefile.sh test/ttcn3/install.sh ./
COPY test/ttcn3/src src
RUN mkdir build && cd build && ../install.sh

# srsUE as TTCN-3 device under test
FROM ubuntu:${OS_VERSION} AS dut
ARG ENABLE_ASAN

ENV DEBIAN_FRONTEND=noninteractive
# Clang links the ASAN runtime statically, so the runtime image doesn't need a matching libasan package
ENV CC=clang CXX=clang++
RUN apt-get update && apt-get install -y --no-install-recommends \
      build-essential clang libclang-rt-dev cmake ninja-build \
      libfftw3-dev libmbedtls-dev libpcsclite-dev libboost-program-options-dev \
      libconfig++-dev libsctp-dev rapidjson-dev \
    && rm -rf /var/lib/apt/lists/*

COPY . /srsran
WORKDIR /srsran/build
# Don't optimize for the build machine, the CI runners that run the image have different CPUs, not all with AVX512.
# ttcn3_dut has no radio, RF_FOUND only avoids that srsUE gets disabled without RF driver.
RUN cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DENABLE_WERROR=OFF -DENABLE_TTCN3=ON -DENABLE_RF_PLUGINS=OFF -DRF_FOUND=True \
      -DGCC_ARCH=x86-64-v3 -DENABLE_AVX512=OFF -DENABLE_ASAN=${ENABLE_ASAN} .. \
    && ninja ttcn3_dut

# Runtime image with the Titan executable and its module libraries, and the DUT
FROM ubuntu:${OS_VERSION}

ENV DEBIAN_FRONTEND=noninteractive
# ttcn3_start needs expect, the Titan main controller libedit, the runtime libxml2 and OpenSSL
RUN apt-get update && apt-get install -y --no-install-recommends \
      expect libedit2 libxml2 libssl3t64 ncat iproute2 procps \
      libfftw3-single3 libmbedtls14t64 libpcsclite1 libboost-program-options1.83.0 \
      libconfig++9v5 libsctp1 libatomic1 \
    && rm -rf /var/lib/apt/lists/*

ENV TTCN3_DIR=/opt/titan
ENV PATH=${TTCN3_DIR}/bin:${PATH}
COPY --from=tester ${TTCN3_DIR} ${TTCN3_DIR}
COPY --from=tester /srsran/test/ttcn3/build/srslte_ttcn3_titan /srsran/test/ttcn3/build/*.so /ttcn3/build/
COPY test/ttcn3/ue_tests.cfg test/ttcn3/start_tc.sh test/ttcn3/end_tc.sh test/ttcn3/run_conformance.sh /ttcn3/
COPY --from=dut /srsran/build/srsue/src/test/ttcn3/src/ttcn3_dut /srsran/build/srsue/src/test/ttcn3/src/

# Titan writes its logs relative to the build directory (see ue_tests.cfg)
RUN ln -s /logs /ttcn3/logs
ENV LD_LIBRARY_PATH=/ttcn3/build:${TTCN3_DIR}/lib
# The test scripts check for ASAN reports in this location, it has no effect on binaries without ASAN
ENV ASAN_OPTIONS=log_path=/logs/asan

WORKDIR /logs
ENTRYPOINT ["/ttcn3/run_conformance.sh"]
CMD ["srsue_conf_testcases"]
