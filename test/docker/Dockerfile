# SPDX-FileCopyrightText: Copyright (C) 2013-2026 Software Radio Systems Limited
# SPDX-License-Identifier: LicenseRef-SRS-Custom

#
# Minimal containers for the ZMQ-based end-to-end tests.
#
# LTE (srsEPC + srsENB + srsUE), build from the repository root and run (needs privileges for the UE netns and TUN):
#   docker build -f test/docker/Dockerfile --target e2e -t srsran-e2e .
#   docker run --rm --privileged -v $PWD/e2e_logs:/logs srsran-e2e 25 1
#
# Intra-eNB handover with two cells (same image, the S1 handover between two eNBs needs Open5GS from the 5G image):
#   docker run --rm --privileged -v $PWD/e2e_logs:/logs --entrypoint /srsran/test/run_lte_ho.sh srsran-e2e /srsran/build intra 25 4
#   docker run --rm --privileged -v $PWD/e2e_logs:/logs --entrypoint /srsran/test/run_lte_ho.sh srsran-e2e-5g /srsran/build s1 25 4
#
# 5G NSA/SA (additionally contains Open5GS for SA):
#   docker build -f test/docker/Dockerfile --target e2e-5g -t srsran-e2e-5g .
#   docker run --rm --privileged -v $PWD/e2e_logs:/logs srsran-e2e-5g nsa_fdd
#

ARG OS_VERSION=22.04

FROM ubuntu:${OS_VERSION} AS builder

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
      build-essential cmake ninja-build \
      libfftw3-dev libmbedtls-dev libpcsclite-dev libboost-program-options-dev \
      libconfig++-dev libsctp-dev libzmq3-dev \
    && rm -rf /var/lib/apt/lists/*

COPY . /srsran
WORKDIR /srsran/build
# Don't optimize for the build machine, the CI runners that run the image have different CPUs, not all with AVX512
RUN cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DENABLE_WERROR=OFF -DENABLE_ZMQ_TEST=ON -DENABLE_RF_PLUGINS=OFF \
      -DGCC_ARCH=x86-64-v3 -DENABLE_AVX512=OFF .. \
    && ninja srsepc srsenb srsue

# Runtime image with only the binaries, example configs and test script, keeping the same layout as the source tree
FROM ubuntu:${OS_VERSION} AS e2e

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
      libfftw3-single3 libmbedcrypto7 libpcsclite1 libboost-program-options1.74.0 \
      libconfig++9v5 libsctp1 libzmq5 \
      iproute2 iputils-ping iperf screen procps \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /srsran/build/srsepc/src/srsepc /srsran/build/srsepc/src/
COPY --from=builder /srsran/build/srsenb/src/srsenb /srsran/build/srsenb/src/
COPY --from=builder /srsran/build/srsue/src/srsue   /srsran/build/srsue/src/
COPY --from=builder /srsran/srsepc/*.example /srsran/srsepc/
COPY --from=builder /srsran/srsenb/*.example /srsran/srsenb/
COPY --from=builder /srsran/srsue/*.example  /srsran/srsue/
COPY --from=builder /srsran/test/run_lte.sh /srsran/test/run_lte_ho.sh /srsran/test/open5gs.sh /srsran/test/

WORKDIR /logs
ENTRYPOINT ["/srsran/test/run_lte.sh", "/srsran/build"]
CMD ["25", "1"]

# Image for the 5G tests, which additionally contains Open5GS as 5GC for SA (build with --target e2e-5g)
FROM e2e AS e2e-5g

RUN apt-get update && apt-get install -y --no-install-recommends software-properties-common gnupg curl ca-certificates \
    && add-apt-repository -y ppa:open5gs/latest \
    && curl -fsSL https://www.mongodb.org/static/pgp/server-7.0.asc | gpg --dearmor -o /usr/share/keyrings/mongodb-7.0.gpg \
    && echo "deb [signed-by=/usr/share/keyrings/mongodb-7.0.gpg] https://repo.mongodb.org/apt/ubuntu jammy/mongodb-org/7.0 multiverse" \
         > /etc/apt/sources.list.d/mongodb-org-7.0.list \
    && apt-get update && apt-get install -y --no-install-recommends open5gs mongodb-org-server mongodb-database-tools \
    && apt-get purge -y software-properties-common && apt-get autoremove -y \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /srsran/test/run_5g.sh /srsran/test/

ENTRYPOINT ["/srsran/test/run_5g.sh", "/srsran/build"]
CMD ["nsa_fdd"]
