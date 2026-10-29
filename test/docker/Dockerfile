#
# Minimal container for the ZMQ-based end-to-end test (srsEPC + srsENB + srsUE).
#
# Build (from the repository root):
#   docker build -f test/docker/Dockerfile -t srsran-e2e .
# Run (needs privileges for the UE netns and TUN devices):
#   docker run --rm --privileged -v $PWD/e2e_logs:/logs srsran-e2e 25 1
#

ARG OS_VERSION=22.04
FROM ubuntu:${OS_VERSION}

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
      build-essential cmake ninja-build \
      libfftw3-dev libmbedtls-dev libpcsclite-dev libboost-program-options-dev \
      libconfig++-dev libsctp-dev libzmq3-dev \
      iproute2 iputils-ping iperf screen procps \
    && rm -rf /var/lib/apt/lists/*

COPY . /srsran
WORKDIR /srsran/build
RUN cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DENABLE_WERROR=OFF -DENABLE_ZMQ_TEST=ON -DENABLE_RF_PLUGINS=OFF .. \
    && ninja srsepc srsenb srsue

WORKDIR /logs
ENTRYPOINT ["/srsran/test/run_lte.sh", "/srsran/build"]
CMD ["25", "1"]
