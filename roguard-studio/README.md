# srsRAN Studio — LTE Network Workbench

Installing on a friend's computer? Start with the [shared fork setup guide](docs/SHARING.md).

A local Next.js workbench and FastAPI control plane for this repository's existing srsRAN 4G build. EPC, eNodeB and UE are native processes; **cells are owned eNodeB configurations**. The direct execution adapter preserves one EPC/eNodeB/cell/UE. The new native FC32 I/Q broker executes two or three co-channel cells across one to three eNodeBs, with one EPC and UE. See [native multi-cell runtime and evidence](docs/MULTICELL_RUNTIME.md). Mobility controls are explicitly planned experiment metadata.

Read the [native audit](docs/NATIVE_AUDIT.md), [capability matrix](docs/CAPABILITIES.md), [current verification](docs/WORKBENCH_VALIDATION.md), and [multi-cell adapter plan](docs/MULTICELL_PLAN.md). The bundled srsEPC does not support S1 handover; cross-eNodeB connected handover requires an independently integrated capable core.

## Start

From the repository root:

```bash
./roguard-studio/scripts/start.sh
```

Open **http://127.0.0.1:3000**. API documentation: http://127.0.0.1:8000/docs. Ctrl+C shuts down Studio and its owned LTE processes. The launcher chooses free ports if 3000 or 8000 is occupied; use its printed URLs; the launcher never stops unrelated services. Run as your ordinary Linux user, not with sudo.

Dependencies have been installed into `roguard-studio/.venv` and `frontend/node_modules`. To reproduce installation on another Ubuntu host:

```bash
python3 -m venv roguard-studio/.venv
roguard-studio/.venv/bin/pip install -r roguard-studio/backend/requirements.lock.txt
npm ci --prefix roguard-studio/frontend
./roguard-studio/scripts/start.sh
```

This uses the existing `build/{srsepc,srsenb,srsue}/src/` executables and `build/lib/src/phy/rf/libsrsran_rf_zmq.so*`. It does not run CMake, reinstall srsRAN, change file capabilities, install a root helper, alter a host namespace or modify upstream C++/configuration examples. The feature branch is `feat/roguard-lte-studio`.

## Full frontend tutorial

Read the [Frontend User Guide and Integration Tutorial](docs/FRONTEND_TUTORIAL.md) for a complete browser walkthrough, component configuration tables, frontend–backend architecture, experiments, troubleshooting, and developer extension instructions.

## First experiment

1. Open Network Builder. Use the starter design or create an empty project.
2. Add EPC, LTE eNodeB and UE processes. Each new eNodeB owns a default Cell. Single-chain S1 and reciprocal RF transport relationships are supplied automatically; these are configuration, never attachment evidence.
3. Expand an eNodeB and select its child Cell. Configure PCI, cell ID, TAC, DL/optional paired UL EARFCN and PRACH root sequence. Bandwidth and ZeroMQ endpoints belong to the eNodeB inspector.
4. Use **Add Cell** inside the eNodeB to design additional cells. Two or three total co-channel cells can execute with the broker; validate the documented limits first. No floating Cell process or security-labeled process is offered.
5. Open **Native files** in the inspector and generate a redacted preview, model-to-file trace and differences against the applied run. Previewing never changes a running configuration.
6. Save and validate, then Start simulation. Readiness and attachment require actual native evidence. Use Simulation Runtime, Native Measurements, Logs & Diagnostics and ZeroMQ / RF Transport for inspection.
7. Switch Designed / Applied / Observed topology layers. The UE serving-cell display matches native attached PCI/EARFCN to a unique applied cell; ECI is not independently measured and ambiguous matches stay unresolved.
8. Edit a supported parameter, save, and restart the full dependency chain. Saving alone leaves the applied configuration and observed serving cell unchanged.
9. Radio Environment shows design coordinates and separately labeled observations. Mobility Experiments saves planned endpoints/speed/timeline; positions do not drive RF samples or invent measurements.
10. Capture and export an experiment; stop the network when finished. Roguard's authenticated integration continues exporting real evidence automatically when configured.

Old projects remain readable: legacy security-labeled process kinds migrate to neutral eNodeBs and flat cell settings become owned cells on read. Original saved records and run evidence are retained. Explicitly removing all cells does not silently create a replacement. Existing scenario/analyst APIs remain compatibility interfaces, while security interpretation belongs to Roguard.

## Isolation prerequisites

The runner uses `unshare --user --map-root-user --net --mount --fork`. It mounts a private `/run`, creates an owned `rgstudio-ue` namespace, and brings up its private loopback. EPC and eNodeB share the private parent network; the UE's TUN is placed in its own namespace using the installed `gw.netns` setting. RF endpoints stay on private loopback. There is no physical radio transmission and no external network route.

`/dev/net/tun`, SCTP, `unshare`, `ip`, `mount`, `stdbuf`, and permitted unprivileged namespaces are required. Socket diagnostics additionally use `ss`. Ubuntu policies, containers and coding-agent sandboxes can restrict user namespaces or hide TUN. Validation displays those restrictions. Run Studio normally from your host terminal; it will not silently switch to a root execution path or alter system security settings.

## Verification

```bash
roguard-studio/.venv/bin/python -m pytest -c roguard-studio/pytest.ini roguard-studio/tests
npm --prefix roguard-studio/frontend run typecheck
npm --prefix roguard-studio/frontend run build
```

The following **opt-in** checks run real LTE and require the prerequisites above:

```bash
roguard-studio/.venv/bin/python roguard-studio/tests/real_smoke.py
roguard-studio/.venv/bin/python roguard-studio/tests/real_reliability.py
# Start Studio in another terminal, then:
npm --prefix roguard-studio/frontend run test:e2e
# For an alternate frontend port: STUDIO_TEST_URL=http://127.0.0.1:<port> npm --prefix roguard-studio/frontend run test:e2e
```

The browser tests use installed Google Chrome by default. Set `ROGUARD_CHROMIUM` to an existing Chromium executable to override it. The real tests generate their own synthetic projects and preserve their evidence in the local workspace. `real_api.py` additionally tests a real PCI change through the API, pre-authorization, analyst approval, redacted export and non-destructive restore.

See [acceptance evidence](docs/ACCEPTANCE.md), [architecture and security](docs/ARCHITECTURE.md), [capabilities and limitations](docs/CAPABILITIES.md), and [measurement provenance](docs/MEASUREMENTS.md). Runtime records and credentials live in the ignored private `var/` directory. Back up that directory while Studio is stopped to preserve your full local workspace; exported experiment archives omit credentials.

## Roguard live integration

Studio exposes an optional authenticated read-only machine interface at `/integration/v1/` (snapshot, durable replay and SSE). Set `STUDIO_INTEGRATION_TOKEN` to an independent random token of at least 32 characters before starting the backend. It cannot access Studio control routes or subscriber/configuration/log exports. The separate Roguard repository's persistent Simulation Bridge connects automatically and serves `/app` in its Customer Dashboard (`/app/simulation` redirects there).

See [integration audit, contracts, configuration, startup and native demo](docs/ROGUARD_INTEGRATION.md) and the [version 1 source event schema](docs/integration-event.v1.schema.json). `STUDIO_DATA_DIR` optionally selects a separate persistence directory; native execution still uses the existing private run directory and single-chain engine.

Studio's default backend also reads `STUDIO_INTEGRATION_TOKEN` from the private `roguard-studio/.env` file when no explicit environment value is supplied. The Roguard setup helper creates matching local credentials; restart an already-running Studio backend once after first configuration. An explicit environment value (including empty to disable integration) takes precedence.
