# Architecture and operating model

## Repository boundaries

- `frontend/`: Next.js App Router, TypeScript, React Flow, Zustand, TanStack Query, ECharts, Framer Motion, Lucide, Tailwind and locally maintained shadcn-style Radix components.
- `backend/app/`: strict Pydantic API models, SQLAlchemy/SQLite storage, discovery, configuration generation, process control and CSV ingestion.
- `orchestrator/runner.py`: isolated Linux lifecycle worker; fixed executable paths and argument arrays, no browser-supplied shell commands.
- `var/`: ignored, owner-only SQLite database and per-run configuration/log/metric directories.
- `configs/upstream-manifest.json`: read-only inspection evidence and upstream example hashes. These originals were not overwritten; no restoration is needed.
- `tests/`: API/security/persistence/parser tests plus explicit real LTE checks.

The existing CMake tree is independent. No upstream C++ file is modified. Disabling or removing `roguard-studio/` leaves the original command-line build and examples usable.

## Owned-cell domain and compatibility

The owned-cell topology schema is `srsran.studio.topology.v2`; unversioned legacy designs migrate on read. `Component.cells[]` contains strict `Cell` objects with `CellConfig`; only an eNodeB can own them. Cell identity, PCI, TAC, EARFCNs, RF port and PRACH root live at this level. Bandwidth, process identity, gains, core bindings and ZeroMQ endpoints belong to the eNodeB. Cell IDs are unique within an eNodeB; topology IDs include children and RF indices are contiguous from zero. The direct adapter executes one cell; the new broker adapter executes two or three total co-channel cells across one to three eNodeBs. Both generators independently refuse unsupported shapes before creating run files. See [native multi-cell runtime](MULTICELL_RUNTIME.md).

Legacy `suspicious`/`temporary` process kinds normalize to `enb`; legacy flat cell parameters migrate only when the `cells` field is absent. This conversion is read-only until the operator saves a new revision. Explicit empty cell lists remain empty and cannot start. The deprecated flat fields are retained for old API clients but owned-cell settings are authoritative. Experiment restore remaps component and cell identities, grouping links and mobility targets.

`Topology.mobility[]` contains planned UE endpoints, speed and duration. It never enters the runtime signature. Cell names and canvas positions also have no native effect. Saving cell configuration changes sets restart-required without modifying the captured applied topology or its private files. Native previews use the actual generator in a disposable directory, redact authentication, normalize temporary paths and produce diffs against sanitized applied files.

The runtime supervisor preserves the original isolated direct runner and adds a manifest-driven co-channel broker branch with separate native configurations, PIDs and telemetry per eNodeB. It reports per-process roles; a cell shares its parent's PID. Current serving-cell association requires a live attached native observation and exactly one matching PCI/EARFCN in the applied topology. This establishes a configuration correlation, not independently measured ECI, PLMN, TAC or handover completion.

The v1 consumer validates runtime payloads strictly. New cell/configuration/serving metadata therefore resides **inside the existing extensible `applied_topology` dictionary**, leaving its established top-level shape intact. Wire projection accepts early development envelopes by moving those extension keys into that dictionary without rewriting stored source evidence or changing source/event IDs, sequence or timestamps. Designed-topology metadata remains explicitly configured, separate from observations. See [integration notes](ROGUARD_INTEGRATION.md).

## Control flow

```mermaid
sequenceDiagram
    participant UI as Browser
    participant API as Local FastAPI
    participant DB as SQLite
    participant Runner as Isolated worker
    participant LTE as EPC / eNB / UE
    UI->>API: Save strict topology and validate
    API->>DB: Save revision and audit event
    UI->>API: Start
    API->>API: Validate graph, RF, binaries, TUN, isolation
    API->>Runner: unshare + fixed private run directory
    Runner->>Runner: Private /run, loopback and UE netns
    Runner->>LTE: Start EPC; wait for MME and SPGW evidence
    Runner->>LTE: Start eNB; wait for ZeroMQ TX listener
    Runner->>LTE: Start UE; verify S1 setup and NAS attach
    Runner-->>API: Real lifecycle and redacted log events
    API->>DB: Measurements, observations and audit trail
    API-->>UI: WebSocket updates and REST state
    UI->>API: Stop
    API->>Runner: Private control pipe stop command
    Runner->>LTE: SIGINT, then bounded TERM/KILL if necessary
    Runner-->>API: Final exit and component states
```

A ZeroMQ eNB may wait for its UE radio peer during initialization. Consequently, the UE starts after the eNB TX listener is evidenced; the runner subsequently requires both S1 completion and successful NAS attachment. It does not deadlock waiting for a peer-dependent eNB initialization before launching that peer. Readiness uses actual logs and `/proc/net/tcp`, with deadlines; short polling intervals are not fixed startup delays.

## Ownership and shutdown

The backend creates a new process session for each supervisor and keeps the subprocess handle and creation time. Graceful shutdown proceeds UE, eNB, EPC. Escalation is limited to that owned process group after checking process identity; it never kills by executable name or arbitrary browser-supplied PID. A child exit marks the run failed and shuts down dependencies. The worker watches its private stdin: backend death closes the pipe and triggers cleanup. `unshare --kill-child` protects the worker when its supervisor dies. Linux releases private mounts/interfaces when the namespace processes exit.

A file lock permits one backend per data directory. Interrupted database records are marked interrupted on startup rather than adopted as running. There is no attempt to adopt unrelated command-line LTE processes.

## Access and privilege boundary

Both services bind to `127.0.0.1`. API mutations require a trusted local Origin; Host checks prevent DNS rebinding. A per-session random HttpOnly SameSite=Strict cookie is required for API reads and controls. WebSocket handshakes check the cookie and Origin. HTTP-only cookies are used because this deployment is localhost HTTP; do not expose this service through an external reverse proxy without adding authenticated TLS access controls and reviewing WebSocket routing.

The backend runs as the ordinary user. The worker maps that user to root **inside a new user namespace**, gaining capabilities only in its private network/mount namespaces. No host-root service, sudo API, arbitrary command endpoint or physical RF driver is installed. Model constraints restrict execution to synthetic `001` PLMNs, loopback addresses, known config keys and ZeroMQ. Argument arrays prevent shell injection; fixed paths constrain generated artifacts. Run directories are private and raw subscriber files have mode 0600. Logs redact authentication-bearing lines and long hex values; exports redact K/OPc and omit subscriber secrets.

The trust boundary is a local research workstation. A malicious process already running as the same Unix user can modify the user's source and private data; this system does not claim to isolate mutually hostile users sharing one account.

## State and persistence

Designed topology includes graph, config and layout. Applied topology is the exact saved document captured at run start. Runtime signatures ignore layout/name/revision-only edits when calculating radio restart requirements. Observed topology is exclusively attached UE PCI/EARFCN evidence; configured PLMN/TAC and designed links are not promoted to observations.

SQLite uses WAL and a busy timeout. Topology revision checking occurs under a write transaction to prevent concurrent-window updates from silently overwriting each other. Events, samples, scenario labels, pre-authorizations, baselines and immutable experiment snapshots are durable. Logs and private engine configs remain per-run files. There is currently no retention policy or long-running ingestion batching; archive or back up old lab data periodically.

A scenario event never modifies measurements. Ground truth, model predictions and analyst decisions have separate fields. The detection model itself is not implemented. An observed mismatch is only a candidate requiring analysis. Pre-authorization is matched to actual measured parameters once; explicit analyst approval is still required to update the baseline. Initial observations without a baseline are labeled baseline-missing, not automatically suspicious.

## Validation report dimensions

Validation keeps three independent results: `valid` and `errors` describe the configured network, `environment_ready` and `runtime_blockers` describe host binaries/plugin/TUN/namespaces/tools, and `adapter_supported` and `adapter_blockers` describe executable adapter limits. `runnable` requires all three. A well-formed two-cell design is valid and remains saveable/previewable, even when the single-cell adapter cannot run it. Missing broker support never changes the host-prerequisites result. Zero owned cells still indicate incomplete eNodeB configuration.

The frontend displays a separate Native execution adapter row and blocker explanation. Native previews use the full generator only for valid, adapter-supported shapes; unsupported multi-cell designs use design-only cell-list generation. Startup still rejects them before any process or run is created.

## Co-channel runtime extension

`radio.py` defines verified shape limits and allocates isolated endpoints; `generate_multi` creates per-process files and `rf-plan.json`. The C++ broker performs FC32 summation/fan-out and reciprocal attenuation with bounded backpressure. Runner readiness requires all native S1 registrations and real UE NAS attachment. Broker/process failure stops owned dependencies. `node_components` maps immutable topology IDs independently from compatibility process roles, avoiding ID/role collisions. Per-eNodeB CSV rows keep role `enb` in v1 while carrying their actual component ID in the envelope. RF plan and broker counters extend the existing open `applied_topology` dictionary, preserving strict v1 consumer compatibility.
