# Install the shared Studio fork

This is RogueBS-Research's fork of upstream srsRAN 4G. Upstream authorship,
history, COPYRIGHT and LICENSE remain intact. Studio is the added workbench
under `roguard-studio/`; the separate Roguard dashboard consumes its evidence.

## Clone this feature branch

```bash
git clone --branch feat/roguard-lte-studio https://github.com/RogueBS-Research/srsRAN_4G.git
cd srsRAN_4G
```

Use a Linux host with a C++ compiler, CMake, Python 3.10 or newer with venv,
Node.js 22.12 or newer and npm. Install native build dependencies for your
distribution; the Ubuntu packages include:

```bash
sudo apt-get update
sudo apt-get install build-essential cmake pkg-config libfftw3-dev \
  libmbedtls-dev libboost-program-options-dev libsctp-dev libconfig++-dev \
  libzmq3-dev python3-venv python3-dev iproute2 util-linux
```

Build the native executables and ZeroMQ plugin locally; binaries are not shared:

```bash
cmake -S . -B build -DENABLE_ZEROMQ=ON -DENABLE_RF_PLUGINS=ON \
  -DENABLE_UHD=OFF -DENABLE_BLADERF=OFF -DENABLE_SOAPYSDR=OFF
cmake --build build --parallel 2
python3 -m venv roguard-studio/.venv
roguard-studio/.venv/bin/pip install -r roguard-studio/backend/requirements.lock.txt
npm ci --prefix roguard-studio/frontend
./roguard-studio/scripts/start.sh
```

The launcher builds Studio's I/Q broker and prints the browser/API addresses.
Run it as your ordinary user. Runtime validation requires `/dev/net/tun`, SCTP
and permission to create unprivileged user/network/mount namespaces. Read the
[Studio guide](../README.md) and [verified multi-cell limits](MULTICELL_RUNTIME.md).
Host-specific namespace restrictions must be resolved before native execution;
the application does not silently elevate privileges.

## Add the Roguard dashboard

Clone the separate repository beside this clone, retaining the directory names
`srsRAN_4G` and `rogue-bs-detection`:

```bash
cd ..
git clone https://github.com/RogueBS-Research/rogue-bs-detection.git
```

Roguard is a private organization repository. Friends need access from its
owner, and must check out the branch containing the live bridge before following
its `docs/simulation-integration.md` and `scripts/start-local.py` instructions.
Studio works independently without access to that repository.

Generate independent local credentials using the Roguard setup helper. Never
copy somebody else's `.env`, private `var/` directory or subscriber credentials.
Restart a running Studio backend after first creating its integration settings.
The dashboard normally uses port 3001 and its bridge uses 8100; Studio normally
uses 3000 and 8000. Use the actual addresses printed by each launcher.

## What is included

Source, dependency lockfiles, tests and recorded acceptance reports are included.
Reports describe prior verification, not a fresh test of the recipient's host.
Build products, installed dependencies, private settings, databases, captures and
native runtime directories are excluded. Existing example subscriber values in
upstream are laboratory examples; generated local subscriber files stay private.

Keep the upstream LICENSE and COPYRIGHT when redistributing this fork.
