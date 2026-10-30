#
# Minimal container for the ZMQ-based end-to-end test (srsEPC + srsENB + srsUE).
#
# Build (from the repository root):
#   docker build -f test/docker/Dockerfile -t srsran-e2e .
# Run (needs privileges for the UE netns and TUN devices):
#   docker run --rm --privileged -v $PWD/e2e_logs:/logs srsran-e2e 25 1
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
RUN cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DENABLE_WERROR=OFF -DENABLE_ZMQ_TEST=ON -DENABLE_RF_PLUGINS=OFF .. \
    && ninja srsepc srsenb srsue

# Runtime image with only the binaries, example configs and test script, keeping the same layout as the source tree
FROM ubuntu:${OS_VERSION}

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
COPY --from=builder /srsran/test/run_lte.sh  /srsran/test/

WORKDIR /logs
ENTRYPOINT ["/srsran/test/run_lte.sh", "/srsran/build"]
CMD ["25", "1"]
