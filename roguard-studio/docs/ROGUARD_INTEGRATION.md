# Native Studio → Roguard integration

## Audit and scope

The actual product repository is `rogue-bs-detection`, beside `srsRAN_4G`. The separate `rogue-bs-web` directory is an older website and is not the integration target.

Before this change, Roguard's `backend/`, `src/`, detection and baseline directories contained plans/READMEs. Its real Next.js customer workspace in `web/` uses deterministic synthetic fixtures and browser-only reviews; it had no persistent backend, authenticated source connector, topology validation service or live API. This change adds those missing simulation-specific services without representing the example detector as operational.

Studio already supplied a native Engine, private generated configuration, one EPC/eNodeB/UE lifecycle, CSV parser, SQLite persistence, local cookie authentication, a lossy in-memory browser WebSocket, and observed PCI/EARFCN baseline review. The integration reuses its parser and lifecycle; it does not rebuild the radio engine or use the privileged browser API as a source credential.

`/app/simulation` is the customer portal's live view. The sidebar links to it from the existing workspace. Synthetic example modules retain their explicit example labels. Actual live measurements and real analyst decisions are persisted in Roguard's backend and do not enter the fixture store. Native eNodeB/UE metrics, runtime, source identity, connection state, observation comparisons, baseline history and approvals appear in the live view. A model prediction remains null; a changed PCI is not itself a confirmed rogue detection.

## Install and configure

Requires Linux with the existing native srsRAN/ZeroMQ build, `/dev/net/tun`, working unprivileged user/network/mount namespaces, Python with the backend requirements, and the website's existing Node.js dependencies. Studio's normal preflight still decides whether native execution is supported.

From the Roguard repository:

```bash
python3 -m venv backend/.venv
backend/.venv/bin/pip install -r backend/requirements.lock.txt
npm ci
python3 scripts/configure-simulation-bridge.py
```

The helper creates private `backend/.env` and `web/.env.local` only when neither exists. It never prints secrets. For existing environments, use `backend/.env.example` and add the server-only `ROGUARD_BACKEND_URL=http://127.0.0.1:8100` and matching `ROGUARD_PORTAL_TOKEN` to `web/.env.local`.

Credentials are separate: `STUDIO_INTEGRATION_TOKEN` grants Studio integration GET access only; `ROGUARD_PORTAL_TOKEN` reads Roguard's API/WebSocket; `ROGUARD_ANALYST_TOKEN` authorizes reviews. Generate independent random values of at least 32 characters. None uses a `NEXT_PUBLIC_` prefix. `ROGUARD_ANALYST_ID` identifies the configured local research analyst in the durable audit. The analyst enters their credential into the live review form; it is held in component memory and cleared after successful review.

Optional `STUDIO_EXPECTED_SOURCE_ID` pins the source UUID from an authenticated Studio snapshot. Without it, the first snapshot enrolls the source UUID permanently in the bridge DB. `SIMULATION_STALE_SECONDS` defaults to 10. `ROGUARD_DATA_DIR` optionally selects an absolute private data directory (default `backend/var`). `STUDIO_DATA_DIR` optionally isolates Studio persistence (default `roguard-studio/var`).

## Start all services

These commands use fixed ports so the connector URL matches Studio. Stop a conflicting instance first. Each block runs in its own terminal.

Studio backend, from `srsRAN_4G`:

```bash
set -a
source ../rogue-bs-detection/backend/.env
set +a
roguard-studio/.venv/bin/python -m uvicorn app.main:app --app-dir roguard-studio/backend --host 127.0.0.1 --port 8000
```

Studio frontend, from `srsRAN_4G`:

```bash
ROGUARD_BACKEND_PORT=8000 npm --prefix roguard-studio/frontend run dev -- --hostname 127.0.0.1 --port 3000
```

Roguard backend, from `rogue-bs-detection`:

```bash
set -a
source backend/.env
set +a
backend/.venv/bin/python -m uvicorn simulation_bridge.main:create_app --factory --app-dir backend --host 127.0.0.1 --port 8100
```

Roguard Customer Dashboard, from `rogue-bs-detection`:

```bash
npm --prefix web run dev -- --hostname 127.0.0.1 --port 3100
```

Open Studio at `http://127.0.0.1:3000` and Roguard at `http://127.0.0.1:3100/app/simulation`. Starting Roguard starts the persistent bridge; no browser connection or export is required for ingestion. Use one backend worker per data directory; a file lock prevents competing bridge writers. For service supervision, run these same commands under your process manager. Shutdown cancels the connector and Studio gracefully stops its owned LTE processes.

## Manual native workflow

1. In Studio, save/validate one EPC, one eNodeB and one reciprocal ZeroMQ UE; start the chain.
2. Roguard automatically enrolls Studio, records the initial snapshot, replays evidence and streams new measurements.
3. Verify native UE attachment, PCI/EARFCN and measurement timestamps in the live customer view. Approve the first native observation with an analyst credential and a reason to establish the Roguard baseline.
4. In Studio change the actual eNodeB PCI from 1 to 2, save and restart the complete chain. A design save alone must not change the observed PCI.
5. Roguard receives the new attached UE observation and creates a pending topology comparison against its approved PCI 1 baseline. Review and approve PCI 2 in the live customer view; the persistent baseline advances only after approval.

Studio's own baseline approvals are independent source analyst events. They never grant Roguard approval. Synthetic scenarios are retained as synthetic evidence and cannot create observed topology baselines. There is no manual CSV/JSON data transfer.

## Contracts and delivery semantics

Machine API, bearer header required, no browser session or Origin:

| Route | Behavior |
| --- | --- |
| `GET /integration/v1/snapshot` | Consistent SQLite snapshot and replay watermark; latest runtime, latest per-component metrics, source UUID, capabilities |
| `GET /integration/v1/events?after=SOURCE:SEQUENCE&limit=200` | Ordered exclusive backfill; maximum 1000 per page; `after=0` begins full history |
| `GET /integration/v1/stream` | SSE `evidence` frames with stable `id`; `Last-Event-ID: SOURCE:SEQUENCE` resumes exclusive replay |
| `GET /integration/v1/schema` | Source event JSON Schema |

Source schema is `srsran.studio.v1`; normalized schema is `roguard.simulation.v1`. Checked-in JSON Schemas in `backend/schemas/` describe both. Every evidence event has `source_id`, monotonic contiguous `sequence`, stable `event_id=source_id:sequence`, UTC ingestion timestamp, timestamp semantics, kind, source type, run/topology identities and projected data. UE engine-relative time is milliseconds; eNodeB time is seconds. Units remain attached to every metric. Null/non-finite source measurements stay null. Raw CSV values and source timestamp provenance are never replaced by chart estimates.

Evidence source types are `native-measurement`, `native-process`, `synthetic-experiment`, `analyst-decision`, and `configuration`. Derived prediction fields remain null. Two-second SSE heartbeats are transport metadata, have no event ID, are not evidence and never advance an ingestion cursor.

The journal write commits in the same SQLite transaction as the source mutation. Bootstrap journals existing safe evidence once, then records every relevant update. Unchanged runtime projections are suppressed. The machine API uses explicit field allowlists: no IMSI, K, OPc, arbitrary logs, file contents, file paths, command execution, lifecycle control or unfiltered topology/configuration objects. Browser cookies do not authenticate the integration API, and bearer headers cannot access Studio controls even alongside a browser cookie. Tokens can be disabled/rotated by restarting the services with changed configuration.

On each session, Roguard first fetches and persists the consistent source snapshot. It then replays from its committed cursor (zero on first enrollment), including history before the snapshot watermark, to build its projections in source order. Snapshot evidence is stored separately; applying a current snapshot over an older cursor would skip historical events, so snapshots never advance the event cursor. SSE starts only after replay catches the snapshot watermark. Events arriving between snapshot/replay/stream are recovered by the journal.

Roguard stores `raw_snapshots`, `raw_events`, `normalized_events`, observations, versioned approved baselines, decisions and checkpoints in `backend/var/bridge.sqlite3`. Raw sanitized source evidence and derived features occupy separate tables. The event ID is unique, and every event's evidence, normalized projection and cursor commit together before that cursor is used as the next `Last-Event-ID` (the durable acknowledgement). Replayed duplicate IDs are harmless only when their payload is identical. Gaps, payload mutations, source changes and database rollbacks are rejected. Reconnect uses bounded exponential backoff with jitter, timeouts, source pinning and no redirected credentials. A source identity/history mismatch remains disconnected until deliberately re-enrolled with a new bridge data directory; existing evidence is preserved.

Roguard API, separate role credentials:

| Route | Credential / behavior |
| --- | --- |
| `GET /api/v1/simulation` | Portal reader; bounded dashboard state and current-run measurements |
| `GET /api/v1/simulation/schema` | Portal reader; source/normalized schemas |
| `WS /api/v1/simulation/ws` | Portal reader via Authorization header; current persisted dashboard state every second |
| `POST /api/v1/simulation/reviews/{event_id}` | Analyst; `{decision:"approve"|"reject",reason:"..."}`; durable audit |

The Next.js same-origin `/api/simulation/state` route reads Roguard using a server-only credential and the customer page refreshes that API every second. Its `/api/simulation/reviews/{event_id}` route forwards the analyst-supplied credential, requires a matching Origin, and exposes only the review action. The source token never reaches a browser. The backend also offers its own authenticated WebSocket for other clients.

A baseline comparison uses only real attached UE-observed PCI/EARFCN, scoped by source and topology. Baseline creation/change requires a pending observation, latest native observation, expected baseline version, analyst credential and meaningful reason. Approval updates the baseline and decision audit atomically. Rejection cannot modify a baseline. Concurrent/repeated/older approvals return 409. Baseline-change comparison is a rule result, separate from source experiment ground truth and absent model predictions.

## Connection state and limits

Connection state comes from the actual bridge session and heartbeat age; a transport heartbeat does not make old measurements fresh. Each component carries its measurement age/stale flag. The live page distinguishes disconnected, connecting/backfilling, unavailable, inactive, stale and live. On portal API failure cached values are explicitly marked last-observed/stale. Stopped runs remain historical evidence. eNodeB CSV can buffer until flush/shutdown, so UE can be live while eNodeB data is unavailable/stale.

This is a localhost research integration. The existing product had no customer authentication/tenant service: this change provides scoped local service credentials and analyst review, not public customer accounts or tenant isolation. APIs enforce localhost hosts and the documented services bind loopback. A remote Studio needs HTTPS through a trusted reverse proxy publishing only `/integration/v1/`, blocking Studio's local `/api/` controls, and preserving its localhost Host restriction; the bridge refuses plain HTTP to non-loopback sources. Public deployment needs real user/tenant authorization integrated with the product before exposing this research portal.

The verified adapter executes **one** native eNodeB and UE. No three-eNodeB execution, neighbors, observed PLMN/TAC, unsupported radio indicators, or detection model is inferred. Source and evidence journals intentionally have no deletion/retention policy yet; preserve the DBs for replay and provision disk for long runs. The first bridge enrollment replays all retained history; a large existing Studio DB may take time to catch up.

## Tests

```bash
# From srsRAN_4G
roguard-studio/.venv/bin/python -m pytest roguard-studio/tests
# From rogue-bs-detection
backend/.venv/bin/python -m pytest backend/tests
npm --prefix web run typecheck
npm --prefix web run lint
npm --prefix web test
```

Protocol/unit tests use clearly marked fixtures, including ordering, duplicate IDs, transaction rollback before acknowledgment, source identity/schema mismatch, restart persistence, freshness, synthetic exclusion and analyst reviews. They do not prove native LTE operation.

To run the real native end-to-end demonstration, install Chromium once (`cd web && npx playwright install chromium`) and run from Roguard with the Studio Python environment (it already has the native backend dependencies):

```bash
../srsRAN_4G/roguard-studio/.venv/bin/python backend/tests/real_lte_demo.py \
  --studio-repo ../srsRAN_4G --output /tmp/roguard-real-lte-result.json
```

This opt-in test starts separate Studio/bridge/portal services with private temporary DBs and ephemeral credentials, launches the real LTE chain, waits for actual UE CSV evidence, verifies the Customer Dashboard in Chromium, approves baseline PCI 1 through the UI, changes applied PCI to 2 with a full-chain restart, verifies the pending topology comparison, and approves PCI 2 through the UI. It fails preflight if tun/namespaces/native binaries are unavailable. It creates no fake radio rows and no predictions. The JSON result contains evidence references and run IDs, never tokens. Services and owned LTE processes are stopped in `finally` cleanup; native run artifacts remain under Studio's existing private run directory.

## Verified native run

The complete native chain, browser metrics display, PCI 1 → PCI 2 restart, pending topology comparison and browser analyst approvals passed on October 30, 2026. See [recorded run IDs and evidence references](roguard-native-demo-results.json). This result establishes one native eNodeB/UE chain only; prediction remains null and multi-cell execution is unverified.

## Primary customer workspace — October 31, 2026

The Roguard primary route is now `/app`, with shared live state across topology, cells, measurements, events, detection availability, approvals, reports and settings. `/app/simulation` redirects there; fixtures are explicitly isolated at `/app/preview`. Studio's source contract adds safe applied graph identities and component IDs without exposing subscriber configuration or lifecycle control. See the sibling product's `docs/customer-live-migration.md` for the full route/component audit, operating architecture and expanded native bridge-restart acceptance results.

## Owned-cell workbench compatibility (31 October 2026)

The `/integration/v1/snapshot`, `/events`, `/stream` and `/schema` endpoints and authentication are preserved. Source UUID, stable event IDs, sequence, replay, units and native/synthetic provenance retain the v1 contract. The existing independent bridge has a strict runtime validator, so new runtime metadata is nested inside the already extensible `data.applied_topology`: `configured_cells`, `configuration_identity`, and nullable `serving_cell`. Its established top-level runtime keys are unchanged. `configured_radio.parameters` reflects the authoritative first owned cell for single-cell compatibility. Additional configured cells are explicitly metadata, never claimed as observed radio neighbors.

`serving_cell` is a unique applied-configuration correlation with attached UE PCI/EARFCN and provenance, not decoded ECI. It becomes null while detached/stopped. Native observation events do not contain automatic security classifications. A design event's additional `configured_cells` is marked configured and cannot establish an observed baseline.

The real native PCI-change run was replayed through the existing bridge with a temporary consumer database, without modifying or deploying the customer application. See [workbench bridge evidence](workbench-bridge-validation.json) and [validation scope](WORKBENCH_VALIDATION.md). These are distinct from earlier historical integration evidence retained in this directory.

## Native co-channel runtime extension

The existing strict v1 runtime envelope is unchanged. Its `components` dictionary additionally carries real `enb-2`, `enb-3` and `broker` states when present. `applied_topology.rf_plan` records actual allocated endpoints/paths and configured attenuation; `applied_topology.broker` carries labeled native I/Q transport counters. Each eNodeB CSV metric retains v1 role `enb` and receives its actual topology `component_id` in the existing envelope field. Native two/three-cell evidence passed authenticated replay and normalization using the unchanged independent Roguard bridge. See [multi-cell bridge evidence](multicell-three-enb-bridge-validation.json).
