# Advanced network topology implementation report

Date: 22 November 2026. Primary checkout: `RogueBS-Research/srsRAN_4G`, branch `feat/roguard-lte-studio`, base commit `16f2eb9fa25ce3a0f20a440e961459a4749d3434`. Changes are uncommitted. Native evidence also records a SHA-256 of the critical implementation because a base commit alone does not identify a modified working tree.

Roguard was inspected at `/mnt/disk-d/joe/Projects/rogue-bs-detection`, branch `feat/admin-command-foundation`, commit `43560e3f790af3f7f4eb3b0b3faae99c30f327b8`; this differs from the requested `main`. Its source was not modified. Its actual FastAPI application, Bridge and SQLite Store are used by the isolated integration harness.

## Executive result

Studio is a software LTE laboratory: a student designs a small 4G network, runs virtual phones/base stations/cores, and inspects the evidence those actual LTE programs produce. The browser provides the controls; native srsRAN programs perform the LTE work; Roguard receives evidence for security analysis.

Studio has gained a bounded native extension rather than a replacement framework. The preserved direct/co-channel adapter remains available. The new `matrix-iq-v2` adapter supplies independent UE receive streams and uplink sources, groups different carrier frequencies into separate brokers, and isolates separate cores. Compatibility, templates and runtime admission use a shared registry with exact-profile evidence.

The native profile suite exercises eleven configurations: five preserved templates and six advanced profiles. The public evidence manifest is `network-evidence/native-profiles.json`; it contains actual native results, not synthetic LTE outputs. Feature verdicts below apply to those exact configurations and binary/implementation versions. The capability matrix records the final execution results and measured resource samples.

Open5GS execution is not implemented. Core selection and prerequisite diagnostics are integrated, but missing host dependencies and unfinished isolated provisioning/lifecycle integration prevent N07 completion. This boundary is explicit; choosing Open5GS never silently starts srsEPC. No handover is claimed.

## How the finished flow works

1. **Design:** Choose a template or add cores, base stations, cells and UEs in Network Builder. This describes the experiment you want.
2. **Check:** Compatibility verifies ownership, unique subscribers, radio settings, adapter limits and available host resources. “Native-verified” requires a matching successful recorded experiment; editable fields alone do not earn that label.
3. **Apply:** Starting creates private native configuration files and launches actual srsEPC/srsENB/srsUE programs. Applied topology records the parameters and identities actually supplied to those programs.
4. **Observe:** Each UE reports its own registration and radio measurements. The broker reports transported samples separately. A configured cell is not automatically an observed neighbor.
5. **Review and preserve:** Inspect each UE, select its measurement series, capture/export an experiment, or restore a separate editable project. Restoring remaps core ownership, cells, radio environments and attenuation references; it stops the active experiment before loading the restored design.
6. **Analyze:** The authenticated bridge sends native evidence to Roguard with component IDs and ordered event identities. Scenario labels remain experiment ground truth rather than radio measurements.

| Term | Plain meaning | Technical role |
|---|---|---|
| UE | The laboratory phone | An independent native srsUE process and subscriber |
| eNodeB / eNB | A 4G base station | A native srsENB process owning one or more LTE cells |
| Cell | One radio service identity | A carrier/cell configuration with PCI and cell identity |
| EPC | The mobile core | Subscriber authentication, registration and packet-network services |
| EARFCN | A numbered LTE frequency | Validated downlink/uplink carrier assignment |
| PRB | A unit of LTE channel capacity | Physical resource blocks determining bandwidth and native sample rate |
| I/Q samples | The actual digital radio waveform | Complex FC32 baseband values transported over ZeroMQ |
| ZeroMQ | The software radio cable | Local socket transport between native RF drivers and brokers |
| Broker | The virtual radio connection | Bounded, aligned weighted mixing within one carrier environment |
| API / SQLite | The application's control interface and saved history | FastAPI endpoints coordinate work; SQLite retains projects, runs, metrics and events |
| Namespace / TUN | An isolated virtual network and interface | Prevents independent cores/UEs from conflicting on addresses or routes |
| S1 / NAS / RRC | Different connection stages | Base station-to-core readiness / UE core registration / radio connection; tracked separately |
| Designed / Applied / Observed | Planned / launched / measured | Separate evidence states; none is substituted for another |

## Baseline and preservation

Before editing, the working tree contained the previous audit report and its evidence directory. Both were preserved. The original Studio SQLite/runtime directories were inventoried and not used by tests. The existing localhost service on port 8100 was not controlled or stopped. Native executables and plugin locations were recorded; srsRAN binaries were copied into a disposable checkout and their original hashes retained.

The prepared baseline passed **60 tests**. Baseline native single-cell, sibling-cell, two-eNodeB, mixed ownership and three-eNodeB experiments were recorded during development. No upstream srsRAN source, native executable, host security policy or physical RF configuration was changed. Only Studio's C++ broker source is extended; normal launch rebuilds that local tool when necessary.

All testing used `/tmp/studio-network-20261122/`, with unique native run directories, dedicated SQLite stores and checked-free loopback service ports. This is necessary because `STUDIO_DATA_DIR` relocates the service database, while the fixed native runner still uses its checkout's private `var/runs` root.

## N01 — Multiple independent UEs

**Before:** One UE process, one subscriber row, scalar attachment/observation, a single UE uplink and receive stream.

**Changed:** Components can explicitly select core/radio environments and reciprocal per-cell path attenuation. UE creation allocates unique synthetic IMSIs. Generation creates a separate random K/OPc pair, native config, log, CSV, RF endpoint pair and TUN namespace for every UE. Subscriber files contain the corresponding independent entries. Authentication secrets remain in private run files and are removed from exports.

The C++ broker reuses its existing ZeroMQ protocol and bounded buffers. Matrix downlink is `DL[u] = sum(g[u,c] * cell[c])`; uplink is `UL[c] = sum(g[u,c] * UE[u])`. Each `UE[u]` originates from its own actual srsUE transmitter. Per-UE streams are neither a cloned uplink nor fabricated measurements. The model is static, reciprocal, sample-aligned and intentionally excludes geometry/Doppler/delay.

The engine stores `ue_states` keyed by immutable component ID, maps actual PIDs from run-local config arguments, ingests each native CSV with that ID, and retains role-specific failures. NAS logs and CSV RRC state are separate. Current serving correlation requires independent NAS success and fresh CSV evidence. Legacy scalar fields remain a first-UE compatibility view. A shared ingestion lock prevents simultaneous capture/monitor threads consuming one native row twice and prevents replacing a run while a reader attributes its rows.

**UI:** Add/inspect multiple UEs, choose core/environment, set path gains, inspect each UE's NAS/RRC/serving/IP evidence, select its measurement series, and inspect all UE log roles. Multiple components do not fall back to another component's runtime state when their own process has not started yet.

**Tests:** Independent FC32 sums/distribution and variable frames; subscriber matching/redaction; duplicate IMSI; per-UE NAS eligibility; native one-cell/two-UE and two-cell/two-UE attachment; UE-2 failure; native browser template and export tests. The two-cell/two-UE profile separately observed PCI 1 and PCI 6.

**Failure policy:** An unexpected UE failure fails the experiment and stops healthy dependents. The failed UE remains failed; a healthy UE is reported stopped, not falsely identified as the failed device. Historical evidence stays attributed to its original UE.

**Files:** `backend/app/models.py`, `network.py`, `configuration.py`, `engine.py`, `integration_contract.py`, `storage.py`; `orchestrator/rf_broker.cpp`, `runner.py`; frontend store/inspector/topology/telemetry/network tools; new tests.

**Plain explanation:** Two phones are now two actual LTE programs with separate subscribers and signals. Each phone has its own evidence, rather than two icons sharing one device.

## N02 — Advanced multi-eNodeB topologies

**Before:** Up to three eNodeBs in one tightly restricted carrier profile.

**Changed:** The extension allocates up to four eNodeB processes, retains each cell's owner, and generates separate native roles/configs/RR files/logs/CSVs. Identities and S1/GTP bindings must be unique within a core environment. The additional four-eNodeB/four-cell and two-UE/two-eNodeB profiles have explicit native tests. Every S1 setup must succeed before the run is ready.

The original five required shapes remain on the legacy adapter. Source support is distinguished from exact acceptance. Invalid identity, sibling PSS and core associations are rejected. The code ceiling is not a claim that arbitrary equal-gain arrangements decode or that more processes are physically impossible.

**Tests:** Five preserved native configurations, additional matrix configurations, ownership/allocation/model tests and native browser regression. Readiness markers are latched from actual logs, so early S1 evidence cannot disappear when a bounded log tail rotates during a longer multi-UE attach.

**Files:** `radio.py` remains the preserved legacy policy; `network.py`, `compatibility.py`, `configuration.py`, runner/engine and runtime UI implement the extension.

**Plain explanation:** Extra base stations now represent separate running programs, each connected to its correct core with its own evidence.

## N03 — More than three cells

**Assessment:** The source carrier maximum is not treated as capacity proof. Sibling cells retain the current three-PSS diversity restriction. Scaling proceeds by distributing four cells across four native eNodeBs. The host has eight logical CPUs; baseline available memory was about 7.4 GB. Queue sizes and process counts are bounded before execution. The private RF paths begin at sample index zero and retain variable input frame boundaries.

**Changed:** The matrix broker admits one to four cell paths, allocation/role guards admit eNodeB 4, and resource admission checks a clearly labeled conservative memory estimate. Missing memory is a host prerequisite blocker, not an invalid LTE parameter. Four-cell admission does not execute a truncated subset. Actual CPU-time deltas and summed RSS are measured per owned process, including broker processes, during acceptance.

**Native result:** Four eNodeBs/four cells execute with S1 readiness, nonzero input/output samples on all paths, real UE NAS attachment and a uniquely correlated serving cell. This proves one controlled four-cell profile. It does not prove simultaneous decoding of every configured cell.

**Limits:** Four total cells is the present implementation/validated ceiling; more than four is not admitted. More than three sibling cells in one eNodeB remains unsupported. Equal-strength arbitrary four-cell decoding and sustained traffic capacity were not certified.

**Files:** `network.py`, registry, broker/runner guards, frontend diagnostics/templates and native resource evidence.

**Plain explanation:** The laboratory can run four actual cell transmitters in one tested arrangement. That does not mean the UE has decoded four neighbor cells.

## N04 — Multiple frequencies

**Before:** One shared carrier baseband in the broker.

**Changed:** Radio environments are keyed by core, EARFCN and bandwidth. Different EARFCNs use separate brokers and independent fixed-frequency UE receivers. There is no mathematical addition of signals centered at different frequencies. One eNodeB must currently keep its owned cells co-channel; separate eNodeBs represent different frequencies.

The registry validates Band 7 range, paired uplink EARFCN, nominal bandwidth margin and receiver/core assignment. The compatibility panel shows band, frequency/channel, PRBs and effective rates; native RF diagnostics show every channel's actual allocated paths and counters. A UE environment assignment is designed information; observed EARFCN still comes from its own native CSV after NAS attachment.

**Tests:** Disjoint endpoint/channel groups, band-edge rejection, invalid pairing, independent per-UE attribution, and native/browser profiles with serving EARFCNs 3350 and 3400. No fabricated cross-frequency neighbor observation is created.

**Limits:** Band 7 only. This is fixed-frequency parallel reception, not inter-frequency scanning, discovery, retuning or handover. Other bands, multiple frequencies within one eNodeB and mixed bandwidth on one carrier remain unsupported.

**Files:** `compatibility.py`, `network.py`, models/generator, matrix runner, inspector/compatibility/RF UI and tests.

**Plain explanation:** Different frequencies use separate virtual radio rooms. A signal from one room cannot accidentally become interference in another.

## N05 — Variable bandwidth

**Before:** Multi-cell broker fixed to 50 PRBs/23.04 Msps transport.

**Changed:** The registry contains the checkout's non-power-of-two FFT mappings: 6/15/25/50/100 PRBs map to 1.92/3.84/5.76/11.52/23.04 Msps. Matrix frames represent one millisecond at the selected rate. Legacy transport remains unchanged. Compatible integer transport/native-rate ratios are distinguished from changing an arbitrary field.

**Native result:** The additional 25-PRB/two-eNodeB profile attaches successfully at 5.76 Msps; the UE's own cell-search log reports PRB 25. The 50-PRB matrix profiles also execute at 11.52 Msps. Exact bandwidth/rate values and generated hashes are recorded.

**Limits:** 6, 15 and 100 are implemented profiles awaiting their own exact native acceptance combinations; they are not blanket-certified by a selector. 75 PRBs remains unsupported. Same-carrier mixed bandwidth is rejected. Different logical carrier groups have separate rates, but arbitrary mixed-rate combinations still need individual acceptance.

**Files:** Registry rate/bandwidth table, generator, broker matrix parser/frame handling, UI and native evidence.

**Plain explanation:** The laboratory has a verified narrower-channel experiment, with a matching sample pipeline rather than only a new dropdown value.

## N06 — Multiple EPC environments

**Before:** One bundled core with one subscriber file and one network environment.

**Changed:** Multiple core components require explicit eNodeB/UE ownership. The first core uses the private parent network; the second uses a separate run-owned network namespace. Each core has its own config/subscriber database/process, and every UE has a distinct TUN namespace. Core addresses, service ports and even private UE IPs may be reused only across the actual namespace boundary.

**Native result:** Two complete independent core/eNodeB/UE chains attach. A separate scoped-shutdown test stops core 2's UEs/broker/eNodeB/EPC while core 1's UE remains attached. Whole experiment stop cleans up all remaining owned children.

**Limits:** Two cores remain the code ceiling. UE count follows the design subject to memory, descriptor and endpoint admission; native verification remains specific to each recorded profile. There is no subscriber exchange, roaming, cross-core handover, dynamic core addition or independent core restart; full restart remains the reconciliation mechanism. The fail-whole-experiment policy applies to unexpected failures; explicit scoped shutdown is distinct.

**Files:** Models/registry, network generator/manifest, matrix runner control protocol, engine/API owned-core stop, UI and tests.

**Plain explanation:** Two separate mobile networks can run inside the same machine without sharing their private networking or subscribers.

## N07 — External EPC integration

**Implemented boundary:** Core-adapter policy records ownership, prerequisites, execution status and handover status. The UI/API can represent an Open5GS selection and explain why it cannot execute. Both automatic and explicitly selected legacy adapters reject it rather than launching a different core.

**Not implemented:** Open5GS native configuration, isolated MongoDB/HSS subscriber provisioning, managed external-core readiness/start/connect/stop, successful native UE attachment and S1 handover. The host lacks Open5GS service binaries and MongoDB. No packages/security settings or pre-existing external core were changed to conceal that dependency.

**Status:** **Blocked by identified technical dependency; external execution remains unimplemented.** The adapter interface and diagnostics are partial groundwork, not a completed Open5GS runtime.

**Tests:** Negative selection/generation/admission tests and srsEPC regressions. No external attach success is claimed.

**References:** [Open5GS quickstart](https://open5gs.org/open5gs/docs/guide/01-quickstart/) describes subscriber/core configuration; [srsRAN S1 handover note](https://docs.srsran.com/projects/4g/en/latest/app_notes/source/handover/source/index.html) explains the need for a capable third-party EPC. Neither substitutes for local native acceptance.

**Plain explanation:** Studio recognizes the external-core option and blocks it honestly. It does not yet run Open5GS.

## N08 — Advanced network templates

**Changed:** A versioned catalog provides purpose, complete topology/ownership, native parameters, required adapter, expected behavior, prerequisites, measured resources, limitations and acceptance references. The five original shapes are available; advanced shapes are published only after matching native acceptance.

Preview and project creation are real API/UI operations. Instantiation gives all project/node/cell/link IDs fresh values, remaps ownership/path references, saves revision 1 and records the originating template/version. Existing projects and active runs are not changed. Editable project fingerprints canonicalize UUIDs, names/coordinates and equivalent numeric encodings; radio/subscriber/path/core changes still invalidate an exact match.

**Tests:** Catalog/admission agreement, immutable templates/fresh IDs, save/reopen, HTTP numeric round trips, native acceptance of every Executable template, browser creation/native start/telemetry/export. The initial browser run exposed the `80` versus `80.0` fingerprint bug; it was corrected with a regression test, not ignored or relabeled as native success.

**Files:** `templates.py`, API routes, `network-tools.tsx`, navigation and tests/evidence.

**Plain explanation:** A template creates a separate working laboratory project, not a screenshot or an overwrite of the student's existing work.

## N09 — Configuration compatibility matrix

**Changed:** `compatibility.py` is the shared admission boundary. It delegates preserved legacy rules and the bounded matrix policy, distinguishes invalid/unsupported/missing prerequisites/code-supported/native-verified states, and is called by validation, generation, start/restart, templates and capability reporting.

Native verification requires a canonical radio/process fingerprint, installed native version and executable hashes, matching compiled broker where needed, and critical implementation SHA-256. Missing/stale broker builds cannot inherit a verified label. Sample-rate, bandwidth margin, core ownership, duplicate identities, count limits and resource prerequisites produce specific diagnostics. Model acceptance alone is never verification.

**Tests:** All five classifications, missing tools/TUN/memory, unsupported core/adapter, invalid LTE parameter, exact evidence/version/hash mismatch, UUID/numeric round-trip stability, template/runtime agreement and preserved shapes. The UI shows the actual design assessment and effective carrier/rate values.

**Files:** Registry/core policy, discovery/generator/engine/API integration, capability UI, templates and tests.

**Plain explanation:** Studio tells the user separately: “your design makes sense,” “our code can attempt it,” and “this exact kind of experiment has actually passed.”

## Architecture, persistence and integration impact

No framework or native srsRAN executable was replaced. The C++ broker keeps legacy invocation and adds a bounded `--matrix` manifest mode. The fixed runner keeps legacy mode and adds matrix orchestration. Optional JSON fields extend the existing topology schema; old records are read with defaults and never bulk-rewritten.

New metrics have stable component IDs. JSON/ZIP preserve them and both CSV export paths now include component/measurement IDs. SQLite snapshots select the latest metric per component rather than collapsing every eNodeB or UE into one role. Existing source IDs, event IDs, ordered sequences and the read-only bearer interface remain backward-compatible. New multi-component runtime data sits inside v1's existing extensible `applied_topology` object; legacy scalar fields remain a primary-UE view.

The real Roguard consumer persists raw and normalized events with component IDs. Its existing latest-metric/baseline UI projections are still role/topology oriented; adapting the separate Roguard presentation/detector scope for multiple UEs is future consumer work. No native threat classification or inter-frequency neighbor observation was invented here.

See [architecture](ADVANCED_NETWORK_ARCHITECTURE.md), [capability matrix](NETWORK_CAPABILITY_MATRIX.md), [developer guide](ADVANCED_NETWORK_DEVELOPER_GUIDE.md) and [test evidence](network-evidence/README.md).

## Testing, corrections and remaining work

Prepared baseline: 60 tests. Expanded backend/broker suite: 84 tests, including real FC32 fixture execution, subscriber protection, compatibility and concurrent ingestion. TypeScript checking passed. All eleven native profiles and both real fault/isolation tests passed. The final browser suite passed all 15 cases with an independently recorded child exit code 0. The unchanged production-build retry passed after an installed Next.js security advisory; the advisory remains a dependency maintenance issue. Exact commands, earlier failures and cleanup are recorded in the evidence ledger and capability matrix.

Development failures are retained: early S1 readiness loss from log-tail rotation, initial legacy test expectations requiring explicit legacy selection, and the numeric fingerprint/browser mismatch. They were investigated and corrected. Later successful execution is separate from those earlier results.

Remaining work is concrete: implement and isolate the external core; provision matching subscribers and prove native attachment; add consumer-side multi-component presentation/baseline scope; acceptance-test other PRB/band/frequency combinations before certifying them; and run sustained traffic, stall/disconnect and longer-duration capacity tests. No completed handover, propagation, geographic motion or roaming is claimed.

The completed bounded profiles support broader student LTE research. They are not a general-purpose cellular simulator or a production network-management platform. Open5GS completion remains a separate blocked gate.
